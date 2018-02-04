object ProfileSampleEdit: TProfileSampleEdit
  Left = 109
  Top = 236
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Profile Sample'
  ClientHeight = 121
  ClientWidth = 209
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
    Left = 21
    Top = 8
    Width = 19
    Height = 13
    Alignment = taRightJustify
    Caption = 'Tag'
  end
  object Label2: TLabel
    Left = 18
    Top = 32
    Width = 22
    Height = 13
    Alignment = taRightJustify
    Caption = 'Start'
  end
  object Label3: TLabel
    Left = 21
    Top = 56
    Width = 19
    Height = 13
    Alignment = taRightJustify
    Caption = 'End'
  end
  object EditTag: TEdit
    Left = 48
    Top = 8
    Width = 153
    Height = 21
    TabOrder = 0
    Text = 'EditTag'
  end
  object EditStart: TEdit
    Left = 48
    Top = 32
    Width = 121
    Height = 21
    TabOrder = 1
    Text = 'EditStart'
  end
  object EditEnd: TEdit
    Left = 48
    Top = 56
    Width = 121
    Height = 21
    TabOrder = 2
    Text = 'EditEnd'
  end
  object ButtonOK: TButton
    Left = 128
    Top = 88
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 3
    OnClick = ButtonOKClick
  end
end
