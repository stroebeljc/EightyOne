object ZX97Dialog: TZX97Dialog
  Left = 327
  Top = 211
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'ZX97 Configuration'
  ClientHeight = 119
  ClientWidth = 336
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Scaled = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 37
    Height = 13
    Caption = 'Protect:'
  end
  object Protect08: TCheckBox
    Left = 56
    Top = 8
    Width = 105
    Height = 17
    Caption = '$0000 - $1FFF'
    Checked = True
    State = cbChecked
    TabOrder = 0
  end
  object ProtectAB: TCheckBox
    Left = 56
    Top = 24
    Width = 105
    Height = 17
    Caption = '$A000 - $BFFF'
    TabOrder = 1
  end
  object ProtectB0: TCheckBox
    Left = 56
    Top = 40
    Width = 105
    Height = 17
    Caption = 'Bank 0 (ROM)'
    Checked = True
    State = cbChecked
    TabOrder = 2
  end
  object ProtectB115: TCheckBox
    Left = 56
    Top = 56
    Width = 105
    Height = 17
    Caption = 'Bank 1-15 (RAM)'
    TabOrder = 3
  end
  object SaveRAM: TCheckBox
    Left = 176
    Top = 8
    Width = 153
    Height = 17
    Alignment = taLeftJustify
    BiDiMode = bdLeftToRight
    Caption = 'Save RAM on power down'
    Checked = True
    ParentBiDiMode = False
    State = cbChecked
    TabOrder = 4
  end
  object OK: TButton
    Left = 173
    Top = 88
    Width = 75
    Height = 25
    Caption = 'OK'
    ModalResult = 1
    TabOrder = 6
  end
  object SwapRAMROM: TCheckBox
    Left = 216
    Top = 24
    Width = 113
    Height = 17
    Alignment = taLeftJustify
    Caption = 'Swap ROM/RAM'
    TabOrder = 5
  end
  object Cancel: TButton
    Left = 253
    Top = 88
    Width = 75
    Height = 25
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 7
    OnClick = CancelClick
  end
end
