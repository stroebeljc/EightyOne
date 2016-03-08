object MemoryWindow: TMemoryWindow
  Left = 451
  Top = 109
  Width = 580
  Height = 302
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  Anchors = [akLeft, akTop, akRight, akBottom]
  BorderStyle = bsSizeToolWin
  Caption = 'Memory'
  Color = clBtnFace
  Constraints.MinHeight = 100
  Constraints.MinWidth = 180
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Courier New'
  Font.Style = []
  OldCreateOrder = False
  PopupMenu = PopupMenu1
  Scaled = False
  OnClick = FormClick
  OnMouseDown = FormMouseDown
  OnMouseMove = FormMouseMove
  OnPaint = FormPaint
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 14
  object ScrollBar1: TScrollBar
    Left = 555
    Top = 0
    Width = 17
    Height = 249
    Align = alRight
    Kind = sbVertical
    LargeChange = 8
    Max = 65536
    PageSize = 0
    TabOrder = 0
    OnChange = ScrollBar1Change
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 249
    Width = 572
    Height = 19
    Panels = <
      item
        Width = 25
      end
      item
        Width = 25
      end
      item
        Width = 25
      end
      item
        Width = 25
      end
      item
        Width = 40
      end>
    SimplePanel = False
  end
  object ButtonFirstChange: TButton
    Left = 0
    Top = 224
    Width = 40
    Height = 19
    Caption = '| <'
    TabOrder = 2
    OnClick = ButtonFirstChangeClick
    OnMouseMove = ButtonChangeMouseMove
  end
  object ButtonPrevChange: TButton
    Tag = 1
    Left = 48
    Top = 224
    Width = 40
    Height = 19
    Caption = '<'
    TabOrder = 3
    OnClick = ButtonPrevChangeClick
    OnMouseMove = ButtonChangeMouseMove
  end
  object ButtonNextChange: TButton
    Tag = 2
    Left = 96
    Top = 224
    Width = 40
    Height = 19
    Caption = '>'
    TabOrder = 4
    OnClick = ButtonNextChangeClick
    OnMouseMove = ButtonChangeMouseMove
  end
  object ButtonLastChange: TButton
    Tag = 3
    Left = 144
    Top = 224
    Width = 40
    Height = 19
    Caption = '> |'
    TabOrder = 5
    OnClick = ButtonLastChangeClick
    OnMouseMove = ButtonChangeMouseMove
  end
  object PopupMenu1: TPopupMenu
    Left = 8
    Top = 8
    object ViewBytes1: TMenuItem
      Caption = 'View Bytes'
      OnClick = ViewBytes1Click
    end
    object ViewWords1: TMenuItem
      Tag = 1
      Caption = 'View Words'
      OnClick = ViewBytes1Click
    end
    object ViewBinary1: TMenuItem
      Tag = 2
      Caption = 'View Binary'
      OnClick = ViewBytes1Click
    end
    object ViewTraditional1: TMenuItem
      Tag = 3
      Caption = 'View Traditional'
      Checked = True
      OnClick = ViewBytes1Click
    end
    object SetAddress1: TMenuItem
      Caption = 'Jump To ...'
      OnClick = SetAddress1Click
    end
    object JumpTo1: TMenuItem
      Caption = 'Jump To'
      Enabled = False
      OnClick = JumpTo1Click
    end
  end
end
