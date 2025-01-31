// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMixerElement.h"

IEMixerElement::IEMixerElement(snd_mixer_elem_t* MixerElementHandle) :
    m_Handle(MixerElementHandle, [](snd_mixer_elem_t* MixerElementHandle)
        {
            if (MixerElementHandle) snd_mixer_elem_free(MixerElementHandle);
        })
{}

std::optional<float> IEMixerElement::GetVolume() const
{
    std::optional<float> Volume;
    if (snd_mixer_elem_t* const Handle = m_Handle.get())
    {
        long Min = 0, Max = 0, VolumeValue = 0;
        snd_mixer_selem_get_playback_volume_range(Handle, &Min, &Max);
        if (snd_mixer_selem_get_playback_volume(Handle, SND_MIXER_SCHN_FRONT_LEFT, &VolumeValue) == 0)
        {
            Volume = static_cast<float>(VolumeValue) / static_cast<float>(Max);
        }
    }
    return Volume;
}

void IEMixerElement::SetVolume(float Volume)
{
    if (snd_mixer_elem_t* const Handle = m_Handle.get())
    {
        long Min = 0, Max = 0;
        snd_mixer_selem_get_playback_volume_range(Handle, &Min, &Max);
        const float FinalVolume = std::clamp(Volume, 0.0f, 1.0f) * Max;
        snd_mixer_selem_set_playback_volume_all(Handle, FinalVolume);
        m_CachedVolume = FinalVolume;
    }
}

std::optional<bool> IEMixerElement::GetMute() const
{
    std::optional<bool> bMute;
    if (snd_mixer_elem_t* const Handle = m_Handle.get())
    {
        int Switch = 0;
        if (snd_mixer_selem_get_playback_switch(Handle, SND_MIXER_SCHN_UNKNOWN, &Switch) == 0)
        {
            bMute = Switch == 0;
        }
    }
    return bMute;
}

void IEMixerElement::SetMute(bool bMute)
{
    if (snd_mixer_elem_t* const Handle = m_Handle.get())
    {
        snd_mixer_selem_set_playback_switch_all(Handle, !bMute);
        m_bCachedMute = bMute;
    }
}

uint32_t IEMixerElement::RegisterCallback(const IEMixerElementCallback& Func, uint8_t CallbackMask, void* UserData)
{
    std::scoped_lock Lock(m_MixerElementCallbackMapCriticalSection);

    const uint32_t CallbackID = std::rand();
    m_RegisteredCallbacks.emplace(CallbackID, std::make_pair(Func, UserData));

    for (IEMixerElementCallbackType Bit = IEMixerElementCallbackType::None;
        Bit < IEMixerElementCallbackType::Size;
        Bit = static_cast<IEMixerElementCallbackType>(static_cast<uint8_t>(Bit) << 1))
    {
        if (CallbackMask & Bit)
        {
            m_CallbackTypeLookupMap[Bit].push_back(CallbackID);
        }
    }
    return CallbackID;
}

void IEMixerElement::UnregisterCallback(uint32_t CallbackID)
{
    m_RegisteredCallbacks.erase(CallbackID);
}

void IEMixerElement::OnMixerEvent()
{
    std::scoped_lock Lock(m_MixerElementCallbackMapCriticalSection);

    const bool CurrentbMute = GetMute().value_or(m_bCachedMute);
    if (CurrentbMute != m_bCachedMute)
    {
        const std::vector<uint32_t>& CallbackIDs = m_CallbackTypeLookupMap[IEMixerElementCallbackType::Mute];
        for (const uint32_t& CallbackID : CallbackIDs)
        {
            const IEMixerElementCallbackMap::iterator& It = m_RegisteredCallbacks.find(CallbackID);
            if (It != m_RegisteredCallbacks.end())
            {
                const std::pair<IEMixerElementCallback, void*>& Callback = It->second;
                Callback.first(*this, IEMixerElementCallbackType::Mute, Callback.second);
            }
        }
        m_bCachedMute = CurrentbMute;
    }
    
    const float CurrentVolume = GetVolume().value_or(m_CachedVolume);
    if (std::fabs(CurrentVolume - m_CachedVolume) > 0.01f)
    {
        const std::vector<uint32_t>& CallbackIDs = m_CallbackTypeLookupMap[IEMixerElementCallbackType::Volume];
        for (const uint32_t& CallbackID : CallbackIDs)
        {
            const IEMixerElementCallbackMap::iterator& It = m_RegisteredCallbacks.find(CallbackID);
            if (It != m_RegisteredCallbacks.end())
            {
                const std::pair<IEMixerElementCallback, void*>& Callback = It->second;
                Callback.first(*this, IEMixerElementCallbackType::Volume, Callback.second);
            }
        }
        m_CachedVolume = CurrentVolume;
    }
}