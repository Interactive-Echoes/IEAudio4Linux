// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMixerElement.h"

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

void IEMixerElement::SetVolume(float Volume) const
{
    if (snd_mixer_elem_t* const Handle = m_Handle.get())
    {
        long Min = 0, Max = 0;
        snd_mixer_selem_get_playback_volume_range(Handle, &Min, &Max);
        float FinalVolume = std::clamp(Volume, 0.0f, 1.0f);
        snd_mixer_selem_set_playback_volume_all(Handle, FinalVolume * Max);
    }
}

std::optional<bool> IEMixerElement::GetMute() const
{
    std::optional<bool> bMute;
    if (snd_mixer_elem_t* const Handle = m_Handle.get())
    {
        int Mute = 0;
        if (snd_mixer_selem_get_playback_switch(Handle, SND_MIXER_SCHN_MONO, &Mute) == 0)
        {
            bMute = Mute != 0;
        }
    }
    return bMute;
}

void IEMixerElement::SetMute(bool bMute) const
{
    if (snd_mixer_elem_t* const Handle = m_Handle.get())
    {
        snd_mixer_selem_set_playback_switch_all(Handle, !bMute);
    }
}

const IEMixerElement::IEMixerElementCallbackMap& IEMixerElement::GetRegisteredCallbacks() const
{
    return m_RegisteredCallbacks;
}

uint32_t IEMixerElement::RegisterCallback(const std::function<void(void*)>& Func, void* UserData)
{
    const uint32_t CallbackID = std::rand();
    m_RegisteredCallbacks.emplace(CallbackID, std::make_pair(Func, UserData));
    return CallbackID;
}

void IEMixerElement::UnregisterCallback(uint32_t CallbackID)
{
    m_RegisteredCallbacks.erase(CallbackID);
}