object BasicLister: TBasicLister
  Left = 190
  Top = 448
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'BASIC Listing'
  ClientHeight = 324
  ClientWidth = 274
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnMouseDown = FormMouseDown
  OnMouseWheel = FormMouseWheel
  OnPaint = FormPaint
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ScrollBar: TScrollBar
    Left = 257
    Top = 30
    Width = 17
    Height = 275
    Align = alRight
    Kind = sbVertical
    PageSize = 0
    TabOrder = 0
    OnChange = ScrollBarChange
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 305
    Width = 274
    Height = 19
    Panels = <
      item
        Width = 140
      end
      item
        Width = 136
      end>
    SimplePanel = False
    SizeGrip = False
  end
  object ToolBar: TToolBar
    Left = 0
    Top = 0
    Width = 274
    Height = 30
    ButtonHeight = 21
    ButtonWidth = 54
    Caption = 'ToolBar'
    EdgeBorders = [ebTop, ebBottom]
    ShowCaptions = True
    TabOrder = 2
    Wrapable = False
    object ToolButtonRefresh: TToolButton
      Left = 0
      Top = 2
      Caption = ' Refresh '
      ImageIndex = 0
      OnClick = ToolButtonRefreshClick
    end
    object ToolButton1: TToolButton
      Left = 54
      Top = 2
      Width = 8
      Caption = 'ToolButton1'
      ImageIndex = 1
      Style = tbsDivider
    end
    object ToolButtonSaveAs: TToolButton
      Left = 62
      Top = 2
      Caption = 'Save As'
      ImageIndex = 4
      OnClick = ToolButtonSaveAsClick
    end
    object ToolButton3: TToolButton
      Left = 116
      Top = 2
      Width = 8
      Caption = 'ToolButton3'
      ImageIndex = 3
      Style = tbsDivider
    end
    object ToolButtonDeselect: TToolButton
      Left = 124
      Top = 2
      Caption = 'Deselect'
      ImageIndex = 2
      OnClick = ToolButtonDeselectClick
    end
    object ToolButton4: TToolButton
      Left = 178
      Top = 2
      Width = 8
      Caption = 'ToolButton4'
      ImageIndex = 5
      Style = tbsDivider
    end
    object ToolButtonLineEnds: TToolButton
      Left = 186
      Top = 2
      Caption = 'Line Ends'
      ImageIndex = 6
      Style = tbsCheck
      OnClick = ToolButtonLineEndsClick
    end
    object ToolButton5: TToolButton
      Left = 240
      Top = 2
      Width = 8
      Caption = 'ToolButton5'
      ImageIndex = 7
      Style = tbsDivider
    end
  end
  object SaveDialog: TSaveDialog
    Left = 8
    Top = 40
  end
end
