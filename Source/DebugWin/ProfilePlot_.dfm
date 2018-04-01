object ProfilePlot: TProfilePlot
  Left = 344
  Top = 139
  Width = 700
  Height = 415
  Caption = 'Profile Plot'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnMouseMove = FormMouseMove
  OnPaint = FormPaint
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 13
  object ScrollBarHorizontal: TScrollBar
    Left = 0
    Top = 341
    Width = 684
    Height = 17
    Align = alBottom
    PageSize = 0
    TabOrder = 0
    OnChange = ScrollBarHorizontalChange
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 358
    Width = 684
    Height = 19
    Panels = <
      item
        Text = 'Min: ---- Max: ----'
        Width = 200
      end
      item
        Text = 'Sample: ----'
        Width = 100
      end
      item
        Text = 'Current: ----'
        Width = 100
      end
      item
        Width = 50
      end>
    SimplePanel = False
  end
end
