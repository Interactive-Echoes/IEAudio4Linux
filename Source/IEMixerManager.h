// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once


#include <iostream>
#include <thread>
#include <unordered_map>
#include <vector>

#include "alsa/asoundlib.h"

#include "IEMixer.h"
#include "IEMixerElement.h"

class IEMixerManager
{
public:
    using IEMixerMap = std::unordered_map<std::string, IEMixer>;
public:
    ~IEMixerManager();

public:
    static IEMixerManager& Get();
    IEMixer& GetIEMixer(const char* CardName);

private:
    static std::unique_ptr<IEMixerManager> m_Instance;
    IEMixerMap m_MixerMap;
    std::vector<std::thread> m_PollingThreads;
};