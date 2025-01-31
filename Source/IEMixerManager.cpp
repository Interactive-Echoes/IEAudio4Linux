// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#include "IEMixerManager.h"

std::unique_ptr<IEMixerManager> IEMixerManager::m_Instance = nullptr;

IEMixerManager& IEMixerManager::Get()
{
    if (!m_Instance)
    {
        m_Instance = std::make_unique<IEMixerManager>();
    }
    return *m_Instance;
}

IEMixer& IEMixerManager::GetIEMixer(const char* CardName)
{
    const IEMixerMap::iterator& It = m_MixerMap.find(CardName);
    if (It != m_MixerMap.end())
    {
        return It->second;
    }
    else
    {
        snd_mixer_t* MixerHandle = nullptr;
        snd_mixer_open(&MixerHandle, 0);
        if (snd_mixer_attach(MixerHandle, CardName) == 0)
        {
            if (snd_mixer_load(MixerHandle) == 0)
            {
                IEMixer& NewMixer = m_MixerMap.emplace(CardName, MixerHandle).first->second;
                return NewMixer;
            }
        }
    }
    const std::string ErrorMsg = std::format("{} mixer not found", CardName);
    throw std::runtime_error(ErrorMsg);
}