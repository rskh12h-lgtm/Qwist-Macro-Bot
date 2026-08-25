#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include "MacroState.hpp"
#include "MacroPopup.hpp"

using namespace geode::prelude;

// pushButton/releaseButton appartiennent a PlayerObject (chaque joueur a son
// propre objet), pas a PlayLayer. On determine si c'est le joueur 1 ou 2 en
// comparant l'objet a PlayLayer::m_player1.
class $modify(MacroPlayerObject, PlayerObject) {
    void pushButton(PlayerButton button) {
        auto& state = MacroState::get();
        if (state.recording) {
            auto pl = PlayLayer::get();
            bool isPlayer1 = !pl || this == pl->m_player1;
            state.record(button, isPlayer1, true);
        }
        PlayerObject::pushButton(button);
    }

    void releaseButton(PlayerButton button) {
        auto& state = MacroState::get();
        if (state.recording) {
            auto pl = PlayLayer::get();
            bool isPlayer1 = !pl || this == pl->m_player1;
            state.record(button, isPlayer1, false);
        }
        PlayerObject::releaseButton(button);
    }
};

class $modify(MacroPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        MacroState::get().resetCounters();
        return true;
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
                auto target = action.isPlayer1 ? this->m_player1 : this->m_player2;
                if (target) {
                    if (action.pressed) {
                        target->pushButton(action.button);
                    } else {
                        target->releaseButton(action.button);
                    }
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
    void keyDown(cocos2d::enumKeyCodes key, double timeStamp) {
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

        PlayLayer::keyDown(key, timeStamp);
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
