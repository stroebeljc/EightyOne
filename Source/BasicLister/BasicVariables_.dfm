object BasicVariables: TBasicVariables
  Left = 180
  Top = 129
  HorzScrollBar.ButtonSize = 1
  HorzScrollBar.Margin = 1
  HorzScrollBar.ThumbSize = 1
  Anchors = [akLeft, akTop, akRight, akBottom]
  BorderIcons = [biMinimize, biMaximize]
  BorderStyle = bsToolWindow
  Caption = 'BASIC Variables'
  ClientHeight = 93
  ClientWidth = 314
  Color = clBtnFace
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
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object StatusBar: TStatusBar
    Left = 0
    Top = 74
    Width = 314
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
end
