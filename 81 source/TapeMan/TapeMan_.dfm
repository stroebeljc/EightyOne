object Tape: TTape
  Left = 620
  Top = 116
  BorderStyle = bsDialog
  Caption = 'Tape Manager'
  ClientHeight = 301
  ClientWidth = 281
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 13
  object DeleteBlock: TButton
    Left = 104
    Top = 256
    Width = 75
    Height = 16
    Anchors = [akRight, akBottom]
    Caption = 'Delete Block'
    TabOrder = 4
    OnClick = DeleteBlockClick
  end
  object ExtractBlock: TButton
    Left = 104
    Top = 232
    Width = 75
    Height = 16
    Anchors = [akRight, akBottom]
    Caption = 'Extract Block'
    TabOrder = 5
    OnClick = ExtractBlockClick
  end
  object SaveTapeBtn: TButton
    Left = 8
    Top = 256
    Width = 75
    Height = 16
    Anchors = [akRight, akBottom]
    Caption = 'Save Tape'
    TabOrder = 2
    OnClick = SaveTapeBtnClick
  end
  object InsertBlock: TButton
    Left = 104
    Top = 208
    Width = 75
    Height = 16
    Anchors = [akRight, akBottom]
    Caption = 'Insert Block'
    TabOrder = 3
    OnClick = InsertBlockClick
  end
  object OK: TButton
    Left = 200
    Top = 256
    Width = 75
    Height = 16
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    Default = True
    TabOrder = 6
    OnClick = OKClick
  end
  object LoadTapeBtn: TButton
    Left = 8
    Top = 232
    Width = 75
    Height = 16
    Anchors = [akRight, akBottom]
    Caption = 'Open Tape'
    TabOrder = 1
    OnClick = LoadTapeBtnClick
  end
  object HeaderControl1: THeaderControl
    Left = 0
    Top = 0
    Width = 281
    Height = 17
    DragReorder = False
    Enabled = False
    FullDrag = False
    Sections = <
      item
        AllowClick = False
        ImageIndex = -1
        MaxWidth = 50
        MinWidth = 50
        Text = 'Cur Pos'
        Width = 50
      end
      item
        ImageIndex = -1
        MaxWidth = 147
        MinWidth = 147
        Text = 'Filename'
        Width = 147
      end
      item
        ImageIndex = -1
        MaxWidth = 82
        MinWidth = 82
        Text = 'Length'
        Width = 82
      end>
  end
  object StringGrid: TStringGrid
    Left = 0
    Top = 17
    Width = 281
    Height = 177
    Anchors = [akLeft, akTop, akRight, akBottom]
    ColCount = 3
    DefaultRowHeight = 18
    FixedCols = 0
    RowCount = 1
    FixedRows = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect]
    ScrollBars = ssVertical
    TabOrder = 0
    OnDblClick = StringGridDblClick
    ColWidths = (
      47
      146
      82)
  end
  object AutoRewind: TCheckBox
    Left = 64
    Top = 279
    Width = 161
    Height = 18
    Anchors = [akRight, akBottom]
    Caption = 'AutoRewind at End of Tape'
    Checked = True
    State = cbChecked
    TabOrder = 7
  end
  object NewTapeBtn: TButton
    Left = 8
    Top = 207
    Width = 75
    Height = 17
    Anchors = [akRight, akBottom]
    Caption = 'New Tape'
    TabOrder = 9
    OnClick = NewTapeBtnClick
  end
  object BlockUp: TButton
    Left = 200
    Top = 208
    Width = 75
    Height = 17
    Anchors = [akRight, akBottom]
    Caption = 'Block Up'
    TabOrder = 10
    OnClick = BlockUpClick
  end
  object BlockDown: TButton
    Left = 200
    Top = 232
    Width = 75
    Height = 17
    Anchors = [akRight, akBottom]
    Caption = 'Block Down'
    TabOrder = 11
    OnClick = BlockDownClick
  end
  object LoadTapeDialog: TOpenDialog
    DefaultExt = '.t81'
    Filter = 'All ZX81 Files|*.t81;*.p|EightyOne Tape Files|*.t81|P Files|*.p'
    Left = 8
    Top = 120
  end
  object SaveTapeDialog: TSaveDialog
    DefaultExt = 'T81'
    Filter = 'EightyOne Tape File|*.T81'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofCreatePrompt, ofEnableSizing]
    Left = 40
    Top = 120
  end
  object InsertBlockDialog: TOpenDialog
    DefaultExt = '.p'
    Filter = '.p Files|*.p|.81 Files|*.81'
    Left = 104
    Top = 120
  end
  object ExtractBlockDialog: TSaveDialog
    DefaultExt = '.p'
    Filter = '.p|*.p'
    Left = 200
    Top = 120
  end
end
