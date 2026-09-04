#include "SoundManager.h"
#include "ResourceType.h"
#include "ResourceTable.h"
#include "ResourceManager.h"
#include <cassert>
#include <algorithm>

SoundManager::SoundManager() :
    m_resourceManager(ResourceManager::GetInstance())
{
}

bool SoundManager::Play(SoundId id) const
{
    auto handle = m_resourceManager.GetHandle(id);

    if (handle == -1)
    {
        assert(false && "リソースとして登録されていないサウンドIDです。");
        return false;
    }

    PlaySoundMem(handle,DX_PLAYTYPE_BACK);
    return true;
}

bool SoundManager::Play(SoundId id, float volume) const
{
    auto handle = m_resourceManager.GetHandle(id);

    if (handle == -1)
    {
        assert(false && "リソースとして登録されていないサウンドIDです。");
        return false;
    }

    auto temp = 255 + std::clamp(volume, 0.0f, 1.0f) / 1.0f;
    auto beforeVolume = GetVolumeSoundMem2(handle);

    ChangeVolumeSoundMem(temp, handle);

    PlaySoundMem(handle, DX_PLAYTYPE_BACK);

    ChangeVolumeSoundMem(beforeVolume, handle);

    return true;
}

bool SoundManager::PlayLoop(SoundId id) const
{
    auto handle = m_resourceManager.GetHandle(id);

    if (handle == -1)
    {
        assert(false && "リソースとして登録されていないサウンドIDです。");
        return false;
    }

    PlaySoundMem(handle,DX_PLAYTYPE_LOOP);
    return true;
}

bool SoundManager::StopLoop(SoundId id) const
{
    auto handle = m_resourceManager.GetHandle(id);

    if (handle == -1)
    {
        assert(false && "リソースとして登録されていないサウンドIDです。");
        return false;
    }

    StopSoundMem(handle);
    return true;
}

bool SoundManager::ChangeVolume(SoundId id, float volume) const
{
    auto handle = m_resourceManager.GetHandle(id);

    if (handle == -1)
    {
        assert(false && "リソースとして登録されていないサウンドIDです。");
        return false;
    }

    auto temp = 255 + std::clamp(volume, 0.0f, 1.0f) / 1.0f;
    ChangeVolumeSoundMem(temp, handle);

    return true;
}
