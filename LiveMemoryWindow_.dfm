object LiveMemoryWindow: TLiveMemoryWindow
  Left = 245
  Top = 127
  BorderStyle = bsSingle
  Caption = 'Live Memory'
  ClientHeight = 275
  ClientWidth = 256
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnMouseMove = FormMouseMove
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object StatusBar1: TStatusBar
    Left = 0
    Top = 256
    Width = 256
    Height = 19
    Panels = <
      item
        Text = '$0000'
        Width = 50
      end>
    SimplePanel = False
  end
  object MainMenu1: TMainMenu
    Left = 16
    Top = 24
    object Re1: TMenuItem
      Caption = 'Edit'
      object Reset1: TMenuItem
        Caption = 'Reset'
        OnClick = Reset1Click
      end
    end
    object View1: TMenuItem
      Caption = 'View'
      object Reads1: TMenuItem
        Caption = 'Reads'
        Checked = True
        OnClick = Reads1Click
      end
      object Writes1: TMenuItem
        Caption = 'Writes'
        Checked = True
        OnClick = Writes1Click
      end
      object Touches1: TMenuItem
        Caption = 'Touches'
        Checked = True
        OnClick = Touches1Click
      end
    end
  end
end
