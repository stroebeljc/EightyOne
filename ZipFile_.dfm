object ZipFile: TZipFile
  Left = 632
  Top = 113
  Width = 192
  Height = 248
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSizeToolWin
  Caption = 'ZipFile'
  Color = clBtnFace
  Constraints.MinHeight = 128
  Constraints.MinWidth = 128
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 68
    Height = 13
    Caption = 'Please Select:'
  end
  object ListBox: TListBox
    Left = 8
    Top = 32
    Width = 168
    Height = 144
    Anchors = [akLeft, akTop, akRight, akBottom]
    ItemHeight = 13
    TabOrder = 0
    OnDblClick = ListBoxDblClick
  end
  object OK: TButton
    Left = 103
    Top = 183
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    TabOrder = 1
    OnClick = OKClick
  end
end
