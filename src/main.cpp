#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "MacroState.hpp"
#include "MacroPopup.hpp"

using namespace geode::prelude;

class $modify(MacroPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        MacroState::get().resetCounters();
        return true;
    }

    void pushButton(PlayerButton button, bool isPlayer1) {
        auto& state = MacroState::get();
        if (state.recording) {
            state.record(button, isPlayer1, true);
        }
        GJBaseGameLayer::pushButton(button, isPlayer1);
    }

    void releaseButton(PlayerButton button, bool isPlayer1) {
        auto& state = MacroState::get();
        if (state.recording) {
            state.record(button, isPlayer1, false);
        }
        GJBaseGameLayer::releaseButton(button, isPlayer1);
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        MacroState::get().resetCounters();
    }

    void update(float dt) {
        auto& state = MacroState::get();

        if (state.playing) {
            while (
                state.playbackIndex < state.actions.size() &&
                state.actions[state.playbackIndex].frame == state.frame
            ) {
                auto& action = state.actions[state.playbackIndex];
                if (action.pressed) {
                    GJBaseGameLayer::pushButton(action.button, action.isPlayer1);
                } else {
                    GJBaseGameLayer::releaseButton(action.button, action.isPlayer1);
                }
                state.playbackIndex++;
            }
        }

        PlayLayer::update(dt);

        if (state.recording || state.playing) {
            state.frame++;
        }

        if (state.playing && state.playbackIndex >= state.actions.size()) {
            if (state.loop) {
                this->resetLevel();
            } else {
                state.playing = false;
            }
        }
    }

    // Raccourcis clavier gardes comme filet de secours (PC uniquement, ignores sur Android)
    void keyDown(cocos2d::enumKeyCodes key) {
        auto& state = MacroState::get();

        switch (key) {
            case cocos2d::KEY_F2:
                state.recording ? state.stopRecording() : state.startRecording();
                return;
            case cocos2d::KEY_F3:
                state.playing ? state.stopPlayback() : state.startPlayback();
                return;
            case cocos2d::KEY_F4:
                state.save(state.lastName);
                return;
            case cocos2d::KEY_F5:
                state.load(state.lastName);
                return;
            default:
                break;
        }

        PlayLayer::keyDown(key);
    }
};

// Bouton du menu pause : ajoute son PROPRE menu independant plutot que de
// chercher un menu existant par ID. Ca garantit que le bouton apparait
// quelle que soit la structure interne de PauseLayer sur ta version de GD.
class $modify(MacroPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto menu = CCMenu::create();
        menu->setID("macro-pause-menu"_spr);
        menu->setPosition({0.f, 0.f});
        this->addChild(menu, 100);

        auto sprite = CircleButtonSprite::create(
            CCSprite::createWithSpriteFrameName("GJ_replayBtn_001.png"),
            CircleBaseColor::Green
        );
        sprite->setScale(0.75f);

        auto btn = CCMenuItemSpriteExtra::create(
            sprite, this, menu_selector(MacroPauseLayer::onOpenMacro)
        );
        btn->setID("macro-pause-button"_spr);
        // Coin haut-gauche de l'ecran pause, zone generalement libre
        btn->setPosition({40.f, winSize.height - 40.f});
        menu->addChild(btn);
    }

    void onOpenMacro(CCObject*) {
        MacroPopup::create()->show();
    }
};
