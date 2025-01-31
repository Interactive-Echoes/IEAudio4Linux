// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMixer.h"

IEMixer::IEMixer(snd_mixer_t* MixerHandle) :
    m_Handle(MixerHandle, [](snd_mixer_t* MixerHandle)
        {
            if (MixerHandle) snd_mixer_close(MixerHandle);
        })
{
    m_PollingThread = std::thread(&IEMixer::PollingFunction, this);
}

IEMixer::~IEMixer()
{
    m_PollingThread.join();
}

IEMixerElement& IEMixer::GetElement(const char* ElementName)
{
    const IEMixerElementMap::iterator& It = m_MixerElementMap.find(ElementName);
    if (It != m_MixerElementMap.end())
    {
        return It->second;
    }
    else if (snd_mixer_t* const Handle = m_Handle.get())
    {
        snd_mixer_selem_id_t* MixerElementID = nullptr;
        snd_mixer_selem_register(Handle, nullptr, nullptr);
        snd_mixer_selem_id_malloc(&MixerElementID);
        snd_mixer_selem_id_set_index(MixerElementID, 0);
        snd_mixer_selem_id_set_name(MixerElementID, ElementName);
        if (snd_mixer_elem_t* const MixerElementHandle = snd_mixer_find_selem(Handle, MixerElementID))
        {
            std::scoped_lock lock(m_MixerElementMapCriticalSection);
            return m_MixerElementMap.emplace(ElementName, MixerElementHandle).first->second;
        }
    }
    const std::string ErrorMsg = std::format("{} mixer element not found", ElementName);
    throw std::runtime_error(ErrorMsg);
}

void IEMixer::PollingFunction()
{
    while (true)
    {
        if (snd_mixer_t* const Handle = m_Handle.get())
        {
            if (snd_mixer_wait(Handle, -1) >= 0)
            {
                snd_mixer_handle_events(Handle);
                std::scoped_lock lock(m_MixerElementMapCriticalSection);
                for (IEMixerElementMap::value_type& Element : m_MixerElementMap)
                {
                    Element.second.OnMixerEvent();
                }
            }
        }
    }
}
