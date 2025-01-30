// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMixer.h"

IEMixerElement& IEMixer::GetElement(const char* ElementName)
{
    const IEMixerElementMap::iterator& It = m_MixerElementMap.find(ElementName);
    if (It != m_MixerElementMap.end())
    {
        return It->second;
    }
    else
    {
        snd_mixer_selem_id_t* MixerElementID = nullptr;
        snd_mixer_selem_id_malloc(&MixerElementID);
        snd_mixer_selem_id_set_index(MixerElementID, 0);
        snd_mixer_selem_id_set_name(MixerElementID, ElementName);
        if (snd_mixer_elem_t* const MixerElementHandle = snd_mixer_find_selem(m_Handle.get(), MixerElementID))
        {
            return m_MixerElementMap.emplace(ElementName, MixerElementHandle).first->second;
        }
    }
    const std::string ErrorMsg = std::format("{} mixer element not found", ElementName);
    throw std::runtime_error(ErrorMsg);
}

void IEMixer::PollingFunction() const
{
    while (true)
    {
        if (snd_mixer_t* const Handle = m_Handle.get())
        {
            snd_mixer_wait(Handle, -1);
            for (const IEMixerElementMap::value_type& Element : m_MixerElementMap)
            {
                // TODO GET EVENT TYPE: Maybe Cache Previous Values?
                for (const IEMixerElement::IEMixerElementCallbackMap::value_type& Callback : Element.second.GetRegisteredCallbacks())
                {
                    Callback.second.first(Callback.second.second);
                }
            }
            snd_mixer_handle_events(Handle);
        }
    }
}
