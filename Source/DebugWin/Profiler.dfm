object Profiler: TProfiler
  Left = 230
  Top = 227
  Width = 492
  Height = 189
  BorderStyle = bsSizeToolWin
  Caption = 'Profiler'
  Color = clBtnFace
  Constraints.MinHeight = 189
  Constraints.MinWidth = 492
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ButtonNew: TButton
    Left = 8
    Top = 122
    Width = 53
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'New'
    TabOrder = 1
    OnClick = ButtonNewClick
  end
  object ButtonDelete: TButton
    Left = 136
    Top = 122
    Width = 53
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Delete'
    Enabled = False
    TabOrder = 3
    OnClick = ButtonDeleteClick
  end
  object ListViewProfileSamples: TListView
    Left = 8
    Top = 8
    Width = 466
    Height = 107
    Anchors = [akLeft, akTop, akRight, akBottom]
    Columns = <
      item
        Caption = 'Tag'
        Width = 132
      end
      item
        Caption = 'Start Addr'
        Width = 60
      end
      item
        Caption = 'End Addr'
        Width = 60
      end
      item
        Caption = 'Min T'
        Width = 70
      end
      item
        Caption = 'Max T'
        Width = 70
      end
      item
        Caption = 'Samples'
        Width = 70
      end>
    ColumnClick = False
    GridLines = True
    HideSelection = False
    RowSelect = True
    TabOrder = 0
    ViewStyle = vsReport
    OnEditing = ListViewProfileSamplesEditing
    OnSelectItem = ListViewProfileSamplesSelectItem
  end
  object ButtonEdit: TButton
    Left = 72
    Top = 122
    Width = 53
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Edit'
    Enabled = False
    TabOrder = 2
    OnClick = ButtonEditClick
  end
  object ButtonReset: TButton
    Left = 200
    Top = 122
    Width = 53
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Reset'
    Enabled = False
    TabOrder = 4
    OnClick = ButtonResetClick
  end
  object ButtonRefresh: TButton
    Left = 358
    Top = 122
    Width = 53
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Refresh'
    Enabled = False
    TabOrder = 6
    OnClick = ButtonRefreshClick
  end
  object ButtonPlot: TButton
    Left = 422
    Top = 122
    Width = 53
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Plot'
    Enabled = False
    TabOrder = 7
    OnClick = ButtonPlotClick
  end
  object AutoRefresh: TCheckBox
    Left = 265
    Top = 126
    Width = 89
    Height = 17
    Anchors = [akRight, akBottom]
    Caption = 'Auto Refresh'
    Enabled = False
    TabOrder = 5
    OnClick = AutoRefreshClick
  end
  object Timer: TTimer
    Enabled = False
    OnTimer = TimerTimer
    Left = 184
    Top = 120
  end
end
