object BasicListingFormatInfoForm: TBasicListingFormatInfoForm
  Left = 196
  Top = 155
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'Saving BASIC Listing As Text'
  ClientHeight = 454
  ClientWidth = 387
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Icon.Data = {
    0000010002002020100000000000E80200002600000010101000000000002801
    00000E0300002800000020000000400000000100040000000000800200000000
    0000000000000000000000000000000000000000800000800000008080008000
    0000800080008080000080808000C0C0C0000000FF0000FF000000FFFF00FF00
    0000FF00FF00FFFF0000FFFFFF00000000000000000000000000000000007777
    7777777777777777777777777777000000000000000000000000000000000000
    0000000000000000000000000000000070070070070070070070070070000000
    7007007007007007007007007000000000000000000000000000000000000000
    0700700700700700700700700700000007007007007007007007007007000000
    0000000000000000000000000000000070070070070070070070070070000000
    7007007007007007007007007000000000000000000000000000000000000007
    0070070070070070070070070000000700700700700700700700700700000000
    0000000000000000000000000000000000000000000000000000000000000777
    7777777777777777777777777770000000000000000000000000000000000099
    9919091999090000000000000000000900019109090900000000000000000000
    9001910191190000000000000000009999190919990900000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000770
    7077777777700000000000000000077777777070777700000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000FFFFFFFF0000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000FFFFFFFFFFFFFFFF280000001000000020000000010004000000
    0000C00000000000000000000000000000000000000000000000000080000080
    00000080800080000000800080008080000080808000C0C0C0000000FF0000FF
    000000FFFF00FF000000FF00FF00FFFF0000FFFFFF0077777777777777770000
    000000000000088878878878878000F7FF7FF7FF7FF00088788788788780F7FF
    7FF7FF7FF0000000000000000000077777777777777000000000000000000999
    0999900000000090909090000000099909999000000000000000000000000000
    0000000000007777777700000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000000000000000000000000000FFFF0000}
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label26: TLabel
    Left = 8
    Top = 40
    Width = 373
    Height = 39
    Caption = 
      'ZX80 and ZX81 inverse characters and ZX80, ZX81 and Spectrum blo' +
      'ck graphic symbols are encoded in accordance with the scheme. No' +
      'n-displayable character codes are output as \nn where nn is the ' +
      'character code expressed'
    WordWrap = True
  end
  object Label27: TLabel
    Left = 8
    Top = 8
    Width = 367
    Height = 26
    Caption = 
      'The BASIC listing format is based upon that used by the ZXText2P' +
      ' utility, with extensions to handle a wider range of scenarios.'
    WordWrap = True
  end
  object Label28: TLabel
    Left = 8
    Top = 80
    Width = 377
    Height = 26
    Caption = 
      'as a 2 digit hexadecimal number. On the Spectrum UDGs are encode' +
      'd as \_n where n is the UDG letter, and a standard \ character i' +
      's encoded as \\.'
    WordWrap = True
  end
  object Label25: TLabel
    Left = 8
    Top = 112
    Width = 369
    Height = 26
    Caption = 
      'Space suppression between consecutive keywords is replicated in ' +
      'the text output, and additional spaces within a line are retaine' +
      'd if present.'
    WordWrap = True
  end
  object Label29: TLabel
    Left = 8
    Top = 145
    Width = 368
    Height = 39
    Caption = 
      'The content of a REM statement is by default output as text if a' +
      'll characters are displayable, and as \nn character codes if any' +
      ' non-displayable characters exist (on the assumption that the RE' +
      'M holds machine code). A configuration'
    WordWrap = True
  end
  object Label30: TLabel
    Left = 8
    Top = 184
    Width = 369
    Height = 26
    Caption = 
      'option allows the contents to always be output in \nn character ' +
      'code format irrespective of whether all characters are displayab' +
      'le.'
    WordWrap = True
  end
  object Label31: TLabel
    Left = 8
    Top = 216
    Width = 369
    Height = 39
    Caption = 
      'The pound symbol is by default output as £, but this is not part' +
      ' of the ASCII character set (it has code $A3). A configuration o' +
      'ption allows it to be output in \nn character code format instea' +
      'd. '
    WordWrap = True
  end
  object Label32: TLabel
    Left = 8
    Top = 310
    Width = 369
    Height = 26
    Caption = 
      'A configuration option allows variable names to be output in low' +
      'ercase to ensure there is no ambiguity with BASIC keywords.'
    WordWrap = True
  end
  object Label33: TLabel
    Left = 8
    Top = 344
    Width = 377
    Height = 33
    Caption = 
      'BASIC listing files are saved with extension B80 for the ZX80, B' +
      '81 for the ZX81 and B82 for the Spectrum.'
    WordWrap = True
  end
  object Label34: TLabel
    Left = 8
    Top = 374
    Width = 285
    Height = 13
    Caption = 'Details of the ZXText2P encoding format can be found here:'
  end
  object Label35: TLabel
    Left = 8
    Top = 390
    Width = 219
    Height = 13
    Caption = 'http://freestuff.grok.co.uk/zxtext2p/index.html'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsUnderline]
    ParentFont = False
    OnClick = Weblink1Click
  end
  object Label36: TLabel
    Left = 8
    Top = 263
    Width = 369
    Height = 39
    Caption = 
      'On the Spectrum the copyright symbol is by default output as ©, ' +
      'but this is not part of the ASCII character set (it has code $A9' +
      '). A configuration option allows it to be output in \nn characte' +
      'r code format instead.'
    WordWrap = True
  end
  object btnOK: TButton
    Left = 160
    Top = 418
    Width = 68
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 0
  end
end
