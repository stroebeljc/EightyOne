object Hardware: THardware
  Left = 439
  Top = 127
  BorderStyle = bsDialog
  Caption = 'Hardware'
  ClientHeight = 206
  ClientWidth = 333
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 88
    Top = 36
    Width = 52
    Height = 13
    Caption = 'RAM Pack'
  end
  object Label2: TLabel
    Left = 104
    Top = 12
    Width = 32
    Height = 13
    Caption = 'Model:'
  end
  object Label3: TLabel
    Left = 96
    Top = 58
    Width = 47
    Height = 13
    Caption = 'ROM File:'
  end
  object RamPack: TComboBox
    Left = 152
    Top = 32
    Width = 81
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 1
    OnChange = RamPackChange
    Items.Strings = (
      'None'
      '2k'
      '4k'
      '8k'
      '16k'
      '32k'
      '48k')
  end
  object ShadowROM: TCheckBox
    Left = 16
    Top = 120
    Width = 145
    Height = 17
    Caption = 'Shadow ROM in 8k-16k'
    TabOrder = 2
    OnClick = ShadowROMClick
  end
  object EnableRAM8k: TCheckBox
    Left = 16
    Top = 136
    Width = 145
    Height = 17
    Caption = 'Enable RAM in 8k-16k'
    Checked = True
    State = cbChecked
    TabOrder = 3
    OnClick = EnableRAM8kClick
  end
  object ShadowRAM: TCheckBox
    Left = 16
    Top = 152
    Width = 145
    Height = 17
    Caption = 'Shadow RAM in 32k-64k'
    Checked = True
    State = cbChecked
    TabOrder = 4
  end
  object ProtectROM: TCheckBox
    Left = 16
    Top = 168
    Width = 145
    Height = 17
    Caption = 'Protect ROM from Writes'
    Checked = True
    State = cbChecked
    TabOrder = 5
  end
  object TrueHRG: TCheckBox
    Left = 176
    Top = 104
    Width = 145
    Height = 17
    Caption = 'True Hires Graphics'
    Checked = True
    State = cbChecked
    TabOrder = 6
    OnClick = TrueHRGClick
  end
  object CheckBox6: TCheckBox
    Left = 176
    Top = 120
    Width = 145
    Height = 17
    Caption = 'Fuller AY Box'
    Enabled = False
    TabOrder = 7
  end
  object CheckBox7: TCheckBox
    Left = 176
    Top = 136
    Width = 145
    Height = 17
    Caption = 'Prism Colour Board'
    Enabled = False
    TabOrder = 8
  end
  object CheckBox8: TCheckBox
    Left = 176
    Top = 152
    Width = 145
    Height = 17
    Caption = 'Kempston Joystick'
    Enabled = False
    TabOrder = 9
  end
  object Button1: TButton
    Left = 256
    Top = 176
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 10
    OnClick = Button1Click
  end
  object Model: TComboBox
    Left = 152
    Top = 8
    Width = 81
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 0
    OnChange = ModelChange
    Items.Strings = (
      'ZX80'
      'ZX81'
      'Jupiter Ace')
  end
  object ROMFileName: TEdit
    Left = 152
    Top = 56
    Width = 81
    Height = 21
    TabOrder = 11
    Text = 'zx81.rom'
  end
  object ROMBrowse: TButton
    Left = 240
    Top = 56
    Width = 25
    Height = 21
    Caption = '...'
    TabOrder = 12
    OnClick = ROMBrowseClick
  end
  object PatchLoad: TCheckBox
    Left = 16
    Top = 88
    Width = 145
    Height = 17
    Caption = 'Patch ROM Load Routine'
    Checked = True
    State = cbChecked
    TabOrder = 13
  end
  object NTSCMachine: TCheckBox
    Left = 176
    Top = 88
    Width = 153
    Height = 17
    Caption = 'NTSC TV'
    TabOrder = 14
  end
  object PatchSave: TCheckBox
    Left = 16
    Top = 104
    Width = 137
    Height = 17
    Caption = 'Patch ROM Save Routine'
    Checked = True
    State = cbChecked
    TabOrder = 15
  end
  object LoadROMDialog: TOpenDialog
    DefaultExt = 'ROM'
    Filter = 'ROM Files|*.rom|BIN Files|*.bin'
    Options = [ofReadOnly, ofHideReadOnly, ofNoChangeDir, ofPathMustExist, ofFileMustExist, ofNoNetworkButton, ofEnableSizing]
    Left = 32
    Top = 8
  end
end
