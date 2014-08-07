﻿#include "BaseFactory.h"

NAME_SPACE_DRAGON_BONES_BEGIN

const std::map<String, DragonBonesData *> &BaseFactory::getDragonBonesDataMap() const
{
    return _dragonBonesDataMap;
}
const std::map<String, ITextureAtlas *> &BaseFactory::getTextureAtlasMap() const
{
    return _textureAtlasMap;
}

BaseFactory::BaseFactory()
{
    autoSearchDragonBonesData = false;
    autoSearchTexture = false;
}
BaseFactory::~BaseFactory()
{
    dispose();
}

DragonBonesData *BaseFactory::getDragonBonesData(const String &name) const
{
    auto iterator = _dragonBonesDataMap.find(name);
    
    if (iterator != _dragonBonesDataMap.end())
    {
        return iterator->second;
    }
    else
    {
        return nullptr;
    }
}
void BaseFactory::addDragonBonesData(DragonBonesData *data, const String &name)
{
    if (!data)
    {
        // throw
    }
    
    const String &key = name.empty() ? data->name : name;
    
    if (key.empty())
    {
        // throw
    }
    
    if (_dragonBonesDataMap.find(key) != _dragonBonesDataMap.end())
    {
        // throw?
        removeDragonBonesData(key, true);
    }
    
    _dragonBonesDataMap[key] = data;
}
void BaseFactory::removeDragonBonesData(const String &name, bool disposeData)
{
    auto iterator = _dragonBonesDataMap.find(name);
    
    if (iterator != _dragonBonesDataMap.end())
    {
        if (disposeData)
        {
            iterator->second->dispose();
            delete iterator->second;
        }
        
        _dragonBonesDataMap.erase(iterator);
    }
}

ITextureAtlas *BaseFactory::getTextureAtlas(const String &name) const
{
    auto iterator = _textureAtlasMap.find(name);
    
    if (iterator != _textureAtlasMap.end())
    {
        return iterator->second;
    }
    else
    {
        return nullptr;
    }
}
void BaseFactory::addTextureAtlas(ITextureAtlas *textureAtlas, const String &name)
{
    if (!textureAtlas)
    {
        // throw
    }
    
    const String &key = name.empty() ? textureAtlas->textureAtlasData->name : name;
    
    if (key.empty())
    {
        // throw
    }
    
    if (_textureAtlasMap.find(key) != _textureAtlasMap.end())
    {
        // throw?
        removeTextureAtlas(key, true);
    }
    
    _textureAtlasMap[key] = textureAtlas;
}
void BaseFactory::removeTextureAtlas(const String &name, bool disposeData)
{
    auto iterator = _textureAtlasMap.find(name);
    
    if (iterator != _textureAtlasMap.end())
    {
        if (disposeData)
        {
            iterator->second->dispose();
            delete iterator->second;
        }
        
        _textureAtlasMap.erase(iterator);
    }
}

void BaseFactory::dispose(bool disposeData)
{
    if (disposeData)
    {
        for (auto iterator = _dragonBonesDataMap.begin(); iterator != _dragonBonesDataMap.end(); ++iterator)
        {
            iterator->second->dispose();
            delete iterator->second;
        }
        
        for (auto iterator = _textureAtlasMap.begin(); iterator != _textureAtlasMap.end(); ++iterator)
        {
            iterator->second->dispose();
            delete iterator->second;
        }
    }
    
    _dragonBonesDataMap.clear();
    _textureAtlasMap.clear();
}

Armature *BaseFactory::buildArmature(const String &armatureName) const
{
    return buildArmature(armatureName, "", armatureName, "", "");
}
Armature *BaseFactory::buildArmature(const String &armatureName, const String &dragonBonesName) const
{
    return buildArmature(armatureName, "", armatureName, dragonBonesName, dragonBonesName);
}
Armature *BaseFactory::buildArmature(const String &armatureName, const String &skinName, const String &animationName, const String &dragonBonesName, const String &textureAtlasName) const
{
    DragonBonesData *dragonBonesData = nullptr;
    ArmatureData *armatureData = nullptr;
    ArmatureData *animationArmatureData = nullptr;
    SkinData *skinData = nullptr;
    SkinData *skinDataCopy = nullptr;
    
    if (!dragonBonesName.empty())
    {
        auto iterator = _dragonBonesDataMap.find(dragonBonesName);
        
        if (iterator != _dragonBonesDataMap.end())
        {
            dragonBonesData = iterator->second;
            armatureData = dragonBonesData->getArmatureData(armatureName);
            _currentDragonBonesDataName = dragonBonesName;
            _currentTextureAtlasName = textureAtlasName.empty() ? _currentDragonBonesDataName : textureAtlasName;
        }
    }
    
    if (!armatureData)
    {
        AutoSearchType searchType = (dragonBonesName.empty() ? AutoSearchType::AST_ALL : (autoSearchDragonBonesData ? AutoSearchType::AST_AUTO : AutoSearchType::AST_NONE));
        
        if (searchType != AutoSearchType::AST_NONE)
        {
            for (auto iterator = _dragonBonesDataMap.begin(); iterator != _dragonBonesDataMap.end(); ++iterator)
            {
                dragonBonesData = iterator->second;
                
                if (searchType == AutoSearchType::AST_ALL || dragonBonesData->autoSearch)
                {
                    armatureData = dragonBonesData->getArmatureData(armatureName);
                    
                    if (armatureData)
                    {
                        _currentDragonBonesDataName = iterator->first;
                        _currentTextureAtlasName = _currentDragonBonesDataName;
                        break;
                    }
                }
            }
        }
    }
    
    if (!armatureData)
    {
        return nullptr;
    }
    
    if (!animationName.empty() && animationName != armatureName)
    {
        animationArmatureData = dragonBonesData->getArmatureData(animationName);
        
        if (!animationArmatureData)
        {
            for (auto iterator = _dragonBonesDataMap.begin(); iterator != _dragonBonesDataMap.end(); ++iterator)
            {
                dragonBonesData = iterator->second;
                animationArmatureData = dragonBonesData->getArmatureData(animationName);
                
                if (animationArmatureData)
                {
                    break;
                }
            }
        }
        
        if (animationArmatureData)
        {
            skinDataCopy = animationArmatureData->getSkinData("");
        }
    }
    
    skinData = armatureData->getSkinData(skinName);
    Armature *armature = generateArmature(armatureData);
    armature->name = armatureName;
    
    if (animationArmatureData)
    {
        armature->getAnimation()->setAnimationDataList(animationArmatureData->animationDataList);
    }
    else
    {
        armature->getAnimation()->setAnimationDataList(armatureData->animationDataList);
    }
    
    //
    buildBones(armature, armatureData);
    
    //
    if (skinData)
    {
        buildSlots(armature, armatureData, skinData, skinDataCopy);
    }
    
    // update armature pose
    armature->advanceTime(0);
    return armature;
}

void *BaseFactory::getTextureDisplay(const String &textureName, const String &textureAtlasName, const DisplayData *displayData) const
{
    ITextureAtlas *textureAtlas = nullptr;
    TextureData *textureData = nullptr;
    
    if (!textureAtlasName.empty())
    {
        auto iterator = _textureAtlasMap.find(textureAtlasName);
        
        if (iterator != _textureAtlasMap.end())
        {
            textureAtlas = iterator->second;
            textureData = textureAtlas->textureAtlasData->getTextureData(textureName);
        }
    }
    
    if (!textureData)
    {
        AutoSearchType searchType = (textureAtlasName.empty() ? AutoSearchType::AST_ALL : (autoSearchTexture ? AutoSearchType::AST_AUTO : AutoSearchType::AST_NONE));
        
        if (searchType != AutoSearchType::AST_NONE)
        {
            for (auto iterator = _textureAtlasMap.begin(); iterator != _textureAtlasMap.end(); ++iterator)
            {
                textureAtlas = iterator->second;
                
                if (searchType == AutoSearchType::AST_ALL || textureAtlas->textureAtlasData->autoSearch)
                {
                    textureData = textureAtlas->textureAtlasData->getTextureData(textureName);
                    
                    if (textureData)
                    {
                        break;
                    }
                }
            }
        }
    }
    
    if (!textureData)
    {
        return nullptr;
    }
    
    if (!displayData)
    {
        auto iterator = _dragonBonesDataMap.find(textureAtlas->textureAtlasData->name);
        
        if (iterator != _dragonBonesDataMap.end())
        {
            DragonBonesData *dragonBonesData = iterator->second;
            
            for (size_t i = 0, l = dragonBonesData->armatureDataList.size(); i < l; ++i)
            {
                for (size_t j = 0, l = dragonBonesData->armatureDataList[i]->skinDataList.size(); j < l; ++j)
                {
                    for (size_t k = 0, l = dragonBonesData->armatureDataList[i]->skinDataList[j]->slotDataList.size(); k < l; ++k)
                    {
                        for (size_t m = 0, l = dragonBonesData->armatureDataList[i]->skinDataList[j]->slotDataList[k]->displayDataList.size(); m < l; ++m)
                        {
                            displayData = dragonBonesData->armatureDataList[i]->skinDataList[j]->slotDataList[k]->displayDataList[m];
                            
                            if (displayData->name != textureName)
                            {
                                displayData = nullptr;
                            }
                            else
                            {
                                break;
                            }
                        }
                        
                        if (displayData)
                        {
                            break;
                        }
                    }
                    
                    if (displayData)
                    {
                        break;
                    }
                }
                
                if (displayData)
                {
                    break;
                }
            }
        }
    }
    
    return generateDisplay(textureAtlas, textureData, displayData);
}

void BaseFactory::buildBones(Armature *armature, const ArmatureData *armatureData) const
{
    for (size_t i = 0, l = armatureData->boneDataList.size(); i < l; ++i)
    {
        const BoneData *boneData = armatureData->boneDataList[i];
        Bone *bone = new Bone();
        bone->name = boneData->name;
        bone->inheritRotation = boneData->inheritRotation;
        bone->inheritScale = boneData->inheritScale;
        // copy
        bone->origin = boneData->transform;
        
        if (armatureData->getBoneData(boneData->parent))
        {
            armature->addBone(bone, boneData->parent);
        }
        else
        {
            armature->addBone(bone);
        }
    }
}

void BaseFactory::buildSlots(Armature *armature, const ArmatureData *armatureData, const SkinData *skinData, const SkinData *skinDataCopy) const
{
    for (size_t i = 0, l = skinData->slotDataList.size(); i < l; ++i)
    {
        SlotData *slotData = skinData->slotDataList[i];
        Bone *bone = armature->getBone(slotData->parent);
        
        if (!bone)
        {
            continue;
        }
        
        Slot *slot = generateSlot(slotData);
        slot->name = slotData->name;
        slot->_originZOrder = slotData->zOrder;
        slot->_slotData = slotData;
        std::vector<std::pair<void *, DisplayType>> displayList;
        void *frameDisplay = nullptr;
        
        for (size_t j = 0, l = slotData->displayDataList.size(); j < l; ++j)
        {
            const DisplayData *displayData = slotData->displayDataList[j];
            
            switch (displayData->type)
            {
                case DisplayType::DT_ARMATURE:
                {
                    DisplayData *displayDataCopy = nullptr;
                    
                    if (skinDataCopy)
                    {
                        const SlotData *slotDataCopy = skinDataCopy->getSlotData(slotData->name);
                        
                        if (slotDataCopy)
                        {
                            displayDataCopy = slotDataCopy->displayDataList[i];
                        }
                    }
                    
                    Armature *childArmature = buildArmature(displayData->name, "", displayDataCopy ? displayDataCopy->name : "", _currentDragonBonesDataName, _currentTextureAtlasName);
                    displayList.push_back(std::make_pair(childArmature, DisplayType::DT_ARMATURE));
                    break;
                }
                
                case DisplayType::DT_IMAGE:
                {
                    void *display = getTextureDisplay(displayData->name, _currentTextureAtlasName, displayData);
                    displayList.push_back(std::make_pair(display, DisplayType::DT_IMAGE));
                    break;
                }
                
                case DisplayType::DT_FRAME:
                {
                    //j
                    //frameDisplay = ;
                    break;
                }
                
                default:
                    break;
            }
        }
        
        bone->addChild(slot);
        
        if (!displayList.empty())
        {
            slot->setDisplayList(displayList, false);
        }
    }
}
NAME_SPACE_DRAGON_BONES_END
