object EditTextForm: TEditTextForm
  Left = 382
  Top = 161
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'Edit Text Block'
  ClientHeight = 90
  ClientWidth = 297
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Scaled = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 24
    Height = 13
    Caption = 'Text:'
  end
  object Edit: TEdit
    Left = 8
    Top = 24
    Width = 281
    Height = 21
    TabOrder = 0
  end
  object OK: TButton
    Left = 112
    Top = 56
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 1
    OnClick = OKClick
  end
end
