object SearchSequence: TSearchSequence
  Left = 302
  Top = 245
  BorderIcons = [biHelp]
  BorderStyle = bsToolWindow
  Caption = 'Find'
  ClientHeight = 81
  ClientWidth = 258
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Scaled = False
  OnKeyPress = FormKeyPress
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 16
    Width = 52
    Height = 13
    Caption = 'Sequence:'
  end
  object EditSequence: TEdit
    Left = 72
    Top = 13
    Width = 177
    Height = 21
    TabOrder = 0
  end
  object Cancel: TButton
    Left = 168
    Top = 48
    Width = 83
    Height = 25
    Caption = 'Cancel'
    TabOrder = 2
    OnClick = CancelClick
  end
  object Find: TButton
    Left = 72
    Top = 48
    Width = 81
    Height = 25
    Caption = 'Find'
    Default = True
    TabOrder = 1
    OnClick = FindClick
  end
end
