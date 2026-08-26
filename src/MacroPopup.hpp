#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>

using namespace geode::prelude;

class MacroPopup : public Popup {
protected:
    // Panneaux
    CCScale9Sprite* m_sidebar = nullptr;
    CCScale9Sprite* m_content = nullptr;
    CCScale9Sprite* m_activePill = nullptr;

    // Contenu
    TextInput* m_nameInput = nullptr;
    CCLabelBMFont* m_statusLabel = nullptr;
    CCLabelBMFont* m_countValue = nullptr;
    ButtonSprite* m_recordSprite = nullptr;
    ButtonSprite* m_playSprite = nullptr;
    CCMenuItemSpriteExtra* m_recordBtn = nullptr;
    CCMenuItemSpriteExtra* m_playBtn = nullptr;
    ButtonSprite* m_loopSprite = nullptr;
    CCMenuItemSpriteExtra* m_loopBtn = nullptr;
    ButtonSprite* m_autoSaveSprite = nullptr;
    CCMenuItemSpriteExtra* m_autoSaveBtn = nullptr;

    // Deplacement (drag) de la fenetre
    bool m_dragging = false;
    CCPoint m_dragTouchStart;
    CCPoint m_dragNodeStart;
    float m_dragZoneHeight = 0.f;

    bool init();
    void refresh();

    CCMenuItemSpriteExtra* addNavItem(CCMenu* menu, char const* label, float y, bool active, SEL_MenuHandler sel);

    void onRecord(CCObject*);
    void onPlay(CCObject*);
    void onSave(CCObject*);
    void onLoad(CCObject*);
    void onLoop(CCObject*);
    void onAutoSave(CCObject*);
    void onSoon(CCObject*);

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override;
    void ccTouchMoved(CCTouch* touch, CCEvent* event) override;
    void ccTouchEnded(CCTouch* touch, CCEvent* event) override;

public:
    static MacroPopup* create();
};
