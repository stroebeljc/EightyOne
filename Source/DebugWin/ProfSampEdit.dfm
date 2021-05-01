object ProfileSampleEdit: TProfileSampleEdit
  Left = 266
  Top = 398
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Profile Entry'
  ClientHeight = 121
  ClientWidth = 246
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Scaled = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 53
    Top = 9
    Width = 19
    Height = 13
    Alignment = taRightJustify
    Caption = 'Tag'
  end
  object Label2: TLabel
    Left = 10
    Top = 33
    Width = 63
    Height = 13
    Alignment = taRightJustify
    Caption = 'Start Address'
  end
  object Label3: TLabel
    Left = 12
    Top = 57
    Width = 60
    Height = 13
    Alignment = taRightJustify
    Caption = 'End Address'
  end
  object Label4: TLabel
    Left = 8
    Top = 87
    Width = 137
    Height = 26
    Caption = 'Specify the addresses of the start and end instructions.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    WordWrap = True
  end
  object EditTag: TEdit
    Left = 80
    Top = 8
    Width = 153
    Height = 21
    TabOrder = 0
    Text = 'EditTag'
  end
  object EditStart: TEdit
    Left = 80
    Top = 32
    Width = 153
    Height = 21
    TabOrder = 1
    Text = 'EditStart'
    OnChange = EditStartChange
    OnDblClick = EditDblClick
  end
  object EditEnd: TEdit
    Left = 80
    Top = 56
    Width = 153
    Height = 21
    TabOrder = 2
    Text = 'EditEnd'
    OnChange = EditEndChange
  end
  object ButtonOK: TButton
    Left = 168
    Top = 88
    Width = 67
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 3
    OnClick = ButtonOKClick
  end
end
