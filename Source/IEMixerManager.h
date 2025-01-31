// SPDX-License-Identifier: GPL-2.0-only
// Copyright © Interactive Echoes. All rights reserved.
// Author: mozahzah

#pragma once


#include <iostream>
#include <unordered_map>
#include <vector>

#include "alsa/asoundlib.h"

#include "IEMixer.h"

class IEMixerManager
{
public:
    using IEMixerMap = std::unordered_map<std::string, IEMixer>;

public:
    static IEMixerManager& Get();
    IEMixer& GetIEMixer(const char* CardName);

private:
    static std::unique_ptr<IEMixerManager> m_Instance;
    IEMixerMap m_MixerMap;
};