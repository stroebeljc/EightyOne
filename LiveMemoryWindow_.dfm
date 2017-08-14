object LiveMemoryWindow: TLiveMemoryWindow
  Left = 197
  Top = 126
  Width = 272
  Height = 294
  Caption = 'LiveMemoryWindow'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object MainMenu1: TMainMenu
    Left = 16
    Top = 24
    object Re1: TMenuItem
      Caption = 'Data'
      object Reset1: TMenuItem
        Caption = 'Reset'
        OnClick = Reset1Click
      end
    end
  end
end
