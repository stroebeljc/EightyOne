object BasicVariables: TBasicVariables
  Left = 186
  Top = 187
  HorzScrollBar.ButtonSize = 1
  HorzScrollBar.Margin = 1
  HorzScrollBar.ThumbSize = 1
  Anchors = [akLeft, akTop, akRight, akBottom]
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'BASIC Variables'
  ClientHeight = 97
  ClientWidth = 312
  Color = clBtnFace
  Constraints.MaxHeight = 1600
  Constraints.MaxWidth = 1000
  Constraints.MinHeight = 80
  Constraints.MinWidth = 80
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  OnMouseDown = FormMouseDown
  OnMouseWheel = FormMouseWheel
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object StatusBar: TStatusBar
    Left = 0
    Top = 78
    Width = 312
    Height = 19
    Panels = <
      item
        Width = 150
      end
      item
        Width = 150
      end>
    SimplePanel = False
    SizeGrip = False
  end
  object ScrollBar: TScrollBar
    Left = 295
    Top = 0
    Width = 17
    Height = 78
    Align = alRight
    Kind = sbVertical
    PageSize = 0
    TabOrder = 1
    OnChange = ScrollBarChange
  end
end
