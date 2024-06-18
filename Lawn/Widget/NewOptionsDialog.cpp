﻿#include "../Board.h"
#include "GameButton.h"
#include "../Cutscene.h"
#include "AlmanacDialog.h"
#include "../LawnCommon.h"
#include "../../LawnApp.h"
#include "../System/Music.h"
#include "../../Resources.h"
#include "NewOptionsDialog.h"
#include "../../ConstEnums.h"
#include "../../Sexy.TodLib/TodFoley.h"
#include "../../SexyAppFramework/Slider.h"
#include "../../SexyAppFramework/Checkbox.h"
#include "../../Sexy.TodLib/TodStringFile.h"

using namespace Sexy;

//0x45C050
NewOptionsDialog::NewOptionsDialog(LawnApp* theApp, bool theFromGameSelector, bool theAdvanced) :
    Dialog(nullptr, nullptr, Dialogs::DIALOG_NEWOPTIONS, true, _S("Options"), _S(""), _S(""), Dialog::BUTTONS_NONE)
{
    mApp = theApp;
    mFromGameSelector = theFromGameSelector;
    mAdvancedMode = theAdvanced;
    SetColor(Dialog::COLOR_BUTTON_TEXT, Color(255, 255, 100));
    mAlmanacButton = MakeButton(NewOptionsDialog::NewOptionsDialog_Almanac, this, _S("[VIEW_ALMANAC_BUTTON]"));
    mRestartButton = MakeButton(NewOptionsDialog::NewOptionsDialog_Restart, this, _S("[RESTART_LEVEL]"));
    mBackToMainButton = MakeButton(NewOptionsDialog::NewOptionsDialog_MainMenu, this, _S("[MAIN_MENU_BUTTON]"));
    mAdvancedButton = MakeButton(NewOptionsDialog::NewOptionsDialog_Advanced, this, _S("[ADVANCED_OPTIONS]"));

    mBackToGameButton = MakeNewButton(
        Dialog::ID_OK,
        this,
        _S("[BACK_TO_GAME]"),
        nullptr,
        IMAGE_OPTIONS_BACKTOGAMEBUTTON0,
        IMAGE_OPTIONS_BACKTOGAMEBUTTON0,
        IMAGE_OPTIONS_BACKTOGAMEBUTTON2
    );
    mBackToGameButton->mTranslateX = 0;
    mBackToGameButton->mTranslateY = 0;
    mBackToGameButton->mTextOffsetX = -2;
    mBackToGameButton->mTextOffsetY = -5;
    mBackToGameButton->mTextDownOffsetX = 0;
    mBackToGameButton->mTextDownOffsetY = 1;
    mBackToGameButton->SetFont(FONT_DWARVENTODCRAFT36GREENINSET);
    mBackToGameButton->SetColor(ButtonWidget::COLOR_LABEL, Color::White);
    mBackToGameButton->SetColor(ButtonWidget::COLOR_LABEL_HILITE, Color::White);
    mBackToGameButton->mHiliteFont = FONT_DWARVENTODCRAFT36BRIGHTGREENINSET;

    mMusicVolumeSlider = new Sexy::Slider(IMAGE_OPTIONS_SLIDERSLOT, IMAGE_OPTIONS_SLIDERKNOB2, NewOptionsDialog::NewOptionsDialog_MusicVolume, this);
    double aMusicVolume = theApp->GetMusicVolume();
    aMusicVolume = max(0.0, min(1.0, aMusicVolume));
    mMusicVolumeSlider->SetValue(aMusicVolume);

    mSfxVolumeSlider = new Sexy::Slider(IMAGE_OPTIONS_SLIDERSLOT, IMAGE_OPTIONS_SLIDERKNOB2, NewOptionsDialog::NewOptionsDialog_SoundVolume, this);
    mSfxVolumeSlider->SetValue(theApp->GetSfxVolume() / 0.65);

    mFullscreenCheckbox = MakeNewCheckbox(NewOptionsDialog::NewOptionsDialog_Fullscreen, this, !theApp->mIsWindowed);
    mHardwareAccelerationCheckbox = MakeNewCheckbox(NewOptionsDialog::NewOptionsDialog_HardwareAcceleration, this, theApp->Is3DAccelerated());
    mDebugModeBox = MakeNewCheckbox(NewOptionsDialog::NewOptionsDialog_DebugMode, this, mApp->mTodCheatKeys);
    mDebugModeBox->SetVisible(false);

    mDiscordBox = MakeNewCheckbox(NewOptionsDialog::NewOptionsDialog_Discord, this, mApp->mDiscordPresence);
    mDiscordBox->SetVisible(false);

    mBankKeybindsBox = MakeNewCheckbox(NewOptionsDialog::NewOptionsDialog_BankKeybinds, this, mApp->mBankKeybinds);
    mBankKeybindsBox->SetVisible(false);

    m09FormatBox = MakeNewCheckbox(NewOptionsDialog::NewOptionsDialog_0_9_Format, this, mApp->mZeroNineBankFormat);
    m09FormatBox->SetVisible(false);

    if (mAdvancedMode)
    {
        mRestartButton->SetVisible(false);
        mAlmanacButton->SetVisible(false);
        mBackToMainButton->SetVisible(false);
        mAdvancedButton->SetVisible(false);
        mBackToGameButton->SetLabel(_S("[DIALOG_BUTTON_OK]"));
    }

    if (mFromGameSelector)
    {
        mRestartButton->SetVisible(false);
        mBackToGameButton->SetLabel(_S("[DIALOG_BUTTON_OK]"));
        if (mApp->HasFinishedAdventure() && !mApp->IsTrialStageLocked())
        {
            mBackToMainButton->SetVisible(false);
            mBackToMainButton->SetLabel(_S("[CREDITS]"));
        }
        else
        {
            mBackToMainButton->SetVisible(false);
            mDiscordBox->SetVisible(false);
            m09FormatBox->SetVisible(false);
            mBankKeybindsBox->SetVisible(false);
        }
    }
    else
    {
        mAdvancedButton->SetVisible(false);
    }

    if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ICE ||
        mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN ||
        mApp->mGameMode == GameMode::GAMEMODE_TREE_OF_WISDOM)
    {
        mRestartButton->SetVisible(false);
    }
    if (mApp->mGameScene == GameScenes::SCENE_LEVEL_INTRO && !mApp->mBoard->mCutScene->IsSurvivalRepick())
    {
        mRestartButton->SetVisible(false);
    }
    if (!mApp->CanShowAlmanac() ||
        mApp->mGameScene == GameScenes::SCENE_LEVEL_INTRO ||
        mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN ||
        mApp->mGameMode == GameMode::GAMEMODE_TREE_OF_WISDOM ||
        mFromGameSelector)
    {
        mAlmanacButton->SetVisible(false);
    }
}

//0x45C760��0x45C780
NewOptionsDialog::~NewOptionsDialog()
{
    delete mMusicVolumeSlider;
    delete mSfxVolumeSlider;
    delete mFullscreenCheckbox;
    delete mHardwareAccelerationCheckbox;
    delete mDebugModeBox;
    delete mDiscordBox;
    delete m09FormatBox;
    delete mBankKeybindsBox;
    delete mAlmanacButton;
    delete mRestartButton;
    delete mBackToMainButton;
    delete mAdvancedButton;
    delete mBackToGameButton;
}

//0x45C880
int NewOptionsDialog::GetPreferredHeight(int theWidth)
{
    return IMAGE_OPTIONS_MENUBACK->mWidth;
}

//0x45C890
void NewOptionsDialog::AddedToManager(Sexy::WidgetManager* theWidgetManager)
{
    Dialog::AddedToManager(theWidgetManager);
    AddWidget(mAlmanacButton);
    AddWidget(mRestartButton);
    AddWidget(mBackToMainButton);
    AddWidget(mAdvancedButton);
    AddWidget(mMusicVolumeSlider);
    AddWidget(mSfxVolumeSlider);
    AddWidget(mHardwareAccelerationCheckbox);
    AddWidget(mDebugModeBox);
    AddWidget(mDiscordBox);
    AddWidget(mBankKeybindsBox);
    AddWidget(m09FormatBox);
    AddWidget(mFullscreenCheckbox);
    AddWidget(mBackToGameButton);
}

//0x45C930
void NewOptionsDialog::RemovedFromManager(Sexy::WidgetManager* theWidgetManager)
{
    Dialog::RemovedFromManager(theWidgetManager);
    RemoveWidget(mAlmanacButton);
    RemoveWidget(mMusicVolumeSlider);
    RemoveWidget(mSfxVolumeSlider);
    RemoveWidget(mFullscreenCheckbox);
    RemoveWidget(mHardwareAccelerationCheckbox);
    RemoveWidget(mDebugModeBox);
    RemoveWidget(mDiscordBox);
    RemoveWidget(mBankKeybindsBox);
    RemoveWidget(m09FormatBox);
    RemoveWidget(mBackToMainButton);
    RemoveWidget(mAdvancedButton);
    RemoveWidget(mBackToGameButton);
    RemoveWidget(mRestartButton);
}

//0x45C9D0
void NewOptionsDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
    Dialog::Resize(theX, theY, theWidth, theHeight);
    mMusicVolumeSlider->Resize(199, 116, 135, 40);
    mSfxVolumeSlider->Resize(199, 143, 135, 40);
    mHardwareAccelerationCheckbox->Resize(283, 175, 46, 45);
    mDebugModeBox->Resize(270, 148, 46, 45);
    mFullscreenCheckbox->Resize(284, 206, 46, 45);
    mDiscordBox->Resize(mDebugModeBox->mX, mDebugModeBox->mY + 40, 46, 45);
    mBankKeybindsBox->Resize(mDiscordBox->mX, mDiscordBox->mY + 40, 46, 45);
    m09FormatBox->Resize(mBankKeybindsBox->mX, mBankKeybindsBox->mY + 40, 46, 45);
    mAlmanacButton->Resize(107, 241, 209, 46);
    mRestartButton->Resize(mAlmanacButton->mX, mAlmanacButton->mY + 43, 209, 46);
    mBackToMainButton->Resize(mRestartButton->mX, mRestartButton->mY + 43, 209, 46);
    mAdvancedButton->Resize(mRestartButton->mX, mRestartButton->mY + 43, 209, 46);
    mBackToGameButton->Resize(30, 381, mBackToGameButton->mWidth, mBackToGameButton->mHeight);

    if (mFromGameSelector)
    {
        mMusicVolumeSlider->mY += 5;
        mSfxVolumeSlider->mY += 10;
        mHardwareAccelerationCheckbox->mY += 15;
        mFullscreenCheckbox->mY += 20;
    }
    if (mAdvancedMode)
    {
        mMusicVolumeSlider->SetVisible(false);
        mSfxVolumeSlider->SetVisible(false);
        mHardwareAccelerationCheckbox->SetVisible(false);
        mFullscreenCheckbox->SetVisible(false);
        mDebugModeBox->SetVisible(true);
        mDiscordBox->SetVisible(true);
        mBankKeybindsBox->SetVisible(true);
        m09FormatBox->SetVisible(true);
    }

    if (mApp->mGameMode == GameMode::GAMEMODE_CHALLENGE_ZEN_GARDEN || mApp->mGameMode == GameMode::GAMEMODE_TREE_OF_WISDOM)
    {
        mAlmanacButton->mY += 43;
    }
}

//0x45CB50
void NewOptionsDialog::Draw(Sexy::Graphics* g)
{
    g->DrawImage(IMAGE_OPTIONS_MENUBACK, 0, 0);

    int aMusicOffset = 0;
    int aSfxOffset = 0;
    int a3DAccelOffset = 0;
    int aFullScreenOffset = 0;
    if (mFromGameSelector)
    {
        aMusicOffset = 5;
        aSfxOffset = 10;
        a3DAccelOffset = 15;
        aFullScreenOffset = 20;
    }
    Sexy::Color aTextColor(107, 109, 145);

    if (!mAdvancedMode)
    {
        TodDrawString(g, _S("Music"), 186, 140 + aMusicOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, _S("Sound FX"), 186, 167 + aSfxOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, _S("3D Acceleration"), 274, 197 + a3DAccelOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, _S("Full Screen"), 274, 229 + aFullScreenOffset, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    }
    else
    {
        TodDrawString(g, mApp->mReconVersion, 210, 137, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_CENTER);
        TodDrawString(g, _S("Debug Mode"), 260, 167, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, _S("Discord Presence"), 260, 207, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, _S("Seedbank Keybinds"), 260, 250, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
        TodDrawString(g, _S("0-9 Keyboard"), 260, 283, FONT_DWARVENTODCRAFT18, aTextColor, DrawStringJustification::DS_ALIGN_RIGHT);
    }

}

//0x45CF50
void NewOptionsDialog::SliderVal(int theId, double theVal)
{
    switch (theId)
    {
    case NewOptionsDialog::NewOptionsDialog_MusicVolume:
        mApp->SetMusicVolume(theVal);
        mApp->mSoundSystem->RehookupSoundWithMusicVolume();
        break;

    case NewOptionsDialog::NewOptionsDialog_SoundVolume:
        mApp->SetSfxVolume(theVal * 0.65);
        mApp->mSoundSystem->RehookupSoundWithMusicVolume();
        if (!mSfxVolumeSlider->mDragging)
        {
            mApp->PlaySample(SOUND_BUTTONCLICK);
        }
        break;
    }
}

//0x45CFF0
void NewOptionsDialog::CheckboxChecked(int theId, bool checked)
{
    switch (theId)
    {
    case NewOptionsDialog::NewOptionsDialog_Fullscreen:
        if (!checked && mApp->mForceFullscreen)
        {
            mApp->DoDialog(
                Dialogs::DIALOG_COLORDEPTH_EXP,
                true,
                _S("No Windowed Mode"),
                _S("Windowed mode is only available if your desktop was running in either\n"
                    "16 bit or 32 bit color mode when you started the game.\n\n"
                    "If you'd like to run in Windowed mode then you need to quit the game and switch your desktop to 16 or 32 bit color mode."),
                _S("OK"),
                Dialog::BUTTONS_FOOTER
            );

            mFullscreenCheckbox->SetChecked(true, false);
        }
        break;

    case NewOptionsDialog::NewOptionsDialog_HardwareAcceleration:
        if (!checked)
        {
            mApp->DoDialog(
                Dialogs::DIALOG_INFO,
                true,
                _S("Warning"),
                _S("Some visual bugs could occur with 3D Acceleration OFF.\n\n"
                    "At the moment they are still being investigated.\n"),
                _S("OK"),
                Dialog::BUTTONS_FOOTER
            );
        }
        else {
            if (!mApp->Is3DAccelerationRecommended())
            {
                mApp->DoDialog(
                    Dialogs::DIALOG_INFO,
                    true,
                    _S("Warning"),
                    _S("Your video card may not fully support this feature.\n\n"
                        "If you experience slower performance, please disable Hardware Acceleration.\n"),
                    _S("OK"),
                    Dialog::BUTTONS_FOOTER
                );
            }
        }
        break;
    }
}

//0x45D290
void NewOptionsDialog::KeyDown(Sexy::KeyCode theKey)
{
    if (mApp->mBoard)
    {
        mApp->mBoard->DoTypingCheck(theKey);
    }

    if (theKey == KeyCode::KEYCODE_SPACE || theKey == KeyCode::KEYCODE_RETURN)
    {
        Dialog::ButtonDepress(Dialog::ID_OK);
    }
    else if (theKey == KeyCode::KEYCODE_ESCAPE)
    {
        Dialog::ButtonDepress(Dialog::ID_CANCEL);
    }
}

//0x45D2F0
void NewOptionsDialog::ButtonPress(int theId)
{
    mApp->PlaySample(SOUND_GRAVEBUTTON);
}

//0x45D310
void NewOptionsDialog::ButtonDepress(int theId)
{
    Dialog::ButtonDepress(theId);
    mApp->isFastMode = false;

    switch (theId)
    {
    case NewOptionsDialog::NewOptionsDialog_Almanac:
    {
        AlmanacDialog* aDialog = mApp->DoAlmanacDialog(SeedType::SEED_NONE, ZombieType::ZOMBIE_INVALID);
        aDialog->WaitForResult(true);
        break;
    }
    case NewOptionsDialog::NewOptionsDialog_Advanced:
    {
        mApp->DoAdvanced();
        break;
    }
    case NewOptionsDialog::NewOptionsDialog_MainMenu:
    {
        if (mFromGameSelector)
        {
            mApp->KillNewOptionsDialog();
            mApp->KillGameSelector();
            mApp->ShowMiniCreditScreen();
        }
        else if (mApp->mBoard && mApp->mBoard->NeedSaveGame())
        {
            mApp->DoConfirmBackToMain();
        }
        else if (mApp->mBoard && mApp->mBoard->mCutScene && mApp->mBoard->mCutScene->IsSurvivalRepick())
        {
            mApp->DoConfirmBackToMain();
        }
        else
        {
            mApp->mBoardResult = BoardResult::BOARDRESULT_QUIT;
            mApp->DoBackToMain();
        }
        break;
    }

    case NewOptionsDialog::NewOptionsDialog_Restart:
    {
        if (mApp->mBoard)
        {
            SexyString aDialogTitle;
            SexyString aDialogMessage;
            if (mApp->IsPuzzleMode())
            {
                aDialogTitle = _S("[RESTART_PUZZLE_HEADER]");
                aDialogMessage = _S("[RESTART_PUZZLE_BODY]");
            }
            else if (mApp->IsChallengeMode())
            {
                aDialogTitle = _S("[RESTART_CHALLENGE_HEADER]");
                aDialogMessage = _S("[RESTART_CHALLENGE_BODY]");
            }
            else if (mApp->IsSurvivalMode())
            {
                aDialogTitle = _S("[RESTART_SURVIVAL_HEADER]");
                aDialogMessage = _S("[RESTART_SURVIVAL_BODY]");
            }
            else
            {
                aDialogTitle = _S("[RESTART_LEVEL_HEADER]");
                aDialogMessage = _S("[RESTART_LEVEL_BODY]");
            }

            LawnDialog* aDialog = (LawnDialog*)mApp->DoDialog(Dialogs::DIALOG_CONFIRM_RESTART, true, aDialogTitle, aDialogMessage, _S(""), Dialog::BUTTONS_YES_NO);
            aDialog->mLawnYesButton->mLabel = TodStringTranslate(_S("RESTART"/*[RESTART_LABEL]*/));
            aDialog->mLawnNoButton->mLabel = TodStringTranslate(_S("[DIALOG_BUTTON_CANCEL]"));

            if (aDialog->WaitForResult(true) == Dialog::ID_YES)
            {
                mApp->mMusic->StopAllMusic();
                mApp->mSoundSystem->CancelPausedFoley();
                mApp->KillNewOptionsDialog();
                mApp->mBoardResult = BoardResult::BOARDRESULT_RESTART;
                mApp->mSawYeti = mApp->mBoard->mKilledYeti;
                if (mApp->mPlayedQuickplay)
                    mApp->StartQuickPlay(mApp->mGameMode, false);
                else
                    mApp->PreNewGame(mApp->mGameMode, false);
            }
        }
        break;
    }

    case NewOptionsDialog::NewOptionsDialog_Update:
        mApp->CheckForUpdates();
        break;
    }
}