#! perl
#---------------------------------------------------------------------------
#  Project:  NitroSDK - xml2env
#  File:     xml2env.pl
#
#  Copyright 2005 Nintendo.  All rights reserved.
#
#  These coded instructions, statements, and computer programs contain
#  proprietary information of Nintendo of America Inc. and/or Nintendo
#  Company Ltd., and are protected by Federal copyright law.  They may
#  not be disclosed to third parties or copied or duplicated in any form,
#  in whole or in part, without the prior written consent of Nintendo.
#
#  $Log: xml2env.pl,v $
#  Revision 1.6  2005/10/27 09:14:50  okubata_ryoma
#  微修正
#
#  Revision 1.5  2005/10/27 08:21:25  okubata_ryoma
#  微修正
#
#  Revision 1.4  2005/10/27 07:53:19  okubata_ryoma
#  xml:parserを使用
#
#  初期版
#
#  $NoKeywords: $
#---------------------------------------------------------------------------


use strict;

# XML::Parserが使用可か調べる
if (eval "require XML::Parser; return 1;" != 1){
    printf "cygwin のセットアップでPerlの最新版とexpatライブラリを入れて、XML::Parserを使用できる環境にしてからもう一度実行してみてください。\n";
    exit;
}

require Getopt::Std;
require XML::Parser;

# ハンドラルーチンへの参照をパラメータに指定して、パーサを初期化する
my $parser = XML::Parser -> new(Handlers => {
        Init =>    \&handle_doc_start,
        Final =>   \&handle_doc_end,
        Start =>   \&handle_elem_start,
        End =>     \&handle_elem_end,
        Char =>    \&handle_char_data,
});
   
my (%opts);
my $c = Getopt::Std::getopts('o:h', \%opts);

my ($src, $tmp, $line, $resource_name, $resource_count);

# 何のオプションもつけなかったときの出力ファイル名
$tmp = $src . '.result.c';


# ｈでヘルプ
if(exists $opts{h} || scalar(@ARGV) < 1){
    print "コマンドの後に引数を１つ(検索したい .xml ファイル)を入力してください。\n";
    print "入力例) perl xml2env.pl d:/test/main.xml \n";
    print "上の例はファイルが d:/test/main.xml にある場合の例です。\n";
    print "引数に\"-o\"を入れ、その後にファイル名を入力するとそのファイルに出力されます。\n";
    print "引数に\"-h\"を入れると、簡単な操作説明が表示されます。\n";
    die "$!";
}



# オプション-o のあとに出力ファイルの名前を入れる
if(exists $opts{o}){
    $tmp = $opts{o};
}

# オプションなしで参照したいファイル名を入れる
if(@ARGV){
    $src = $ARGV[$resource_count];
}

open(FO, ">" . $tmp) or die ("cannot open $tmp to write.");

my %default_type = (
 "u8"         => "ENV_U8",
 "s8"         => "ENV_S8",
 "u16"        => "ENV_U16",
 "s16"        => "ENV_S16",
 "u32"        => "ENV_U32",
 "s32"        => "ENV_S32",
 "u64"        => "ENV_U64",
 "s64"        => "ENV_S64",
 "bool"       => "ENV_BOOL",
 "string"     => "ENV_STRING",
 "hexbinary"  => "ENV_BINARY"
);

$line = "/*---------------------------------------------------------------------------*/\n";
$line .= "#include <nitro.h>\n";
$line .= "//================================================================================\n";
$line .= "\n";
print   FO  $line;


# データを読み込みパーサで処理する
my $file = $src;
if($file){
    $parser->parsefile($file);
}
else{
    my $input = "";
    while(<STDIN>){ $input .= $_; }
    $parser->parse($input);
}

close(FO);
exit;

my ($record, $context, $resource_names, $datafile_name);

# ハンドラ

# 処理が始まると同時に、ファイルの先頭を出力する。
sub handle_doc_start{
    print "";
}
my ($class_name, $class_context);
# パーサが新しい要素の開始に出会うたびに呼ばれる。
sub handle_elem_start{
    my($expat, $name, %atts) = @_;
    $context = $name;
    
    if($name =~ /resource/i){
        $resource_names .= " $atts{'name'},";
        print "ENVResource $atts{'name'}\[\] = {\n";
        print   FO  "ENVResource $atts{'name'}\[\] = {\n";
    }
    elsif($name =~ /class/i){
        $class_name = $atts{'name'};
    }
    else{
        # ループでdefault_typeに当てはまるモノを探して処理
        foreach my $i(keys %default_type){
            if($name =~ /$i/i){
                $class_context = "\"$class_name\.$atts{'name'}\",\t". "$default_type{$name}";
                if($name =~ /hexbinary/i){
                    if(exists $atts{'src'}){
                        $datafile_name = $atts{'src'};
                        # ファイル読み込み用の属性値の初期化
                        $atts{'src'} = undef;
                    }
                }
            }
        }
    }
}

# 要素内の文字データを最新要素のバッファに収納する。
sub handle_char_data{
    my($expat, $text) = @_;
    
    $record->{$context} .= $text;
    
}
my ($line, $filedata, $char);
# パーサが新しい要素の終了に出会うたびに呼ばれる。
sub handle_elem_end{
    my($expat, $name) = @_;
    if($name =~ /resource/i){
        print "    ENV_RESOUCE_END\n};\n\n";
        print   FO  "    ENV_RESOUCE_END\n};\n\n";
    }
    # 要素名がclassでないとき
    elsif($name ne 'class'){
        if($name =~ /hexbinary/i){
            # 要素名がhexbinaryでかつ、ファイルを読み込む必要があるとき
            if($datafile_name){
                open(FP, "<" . $datafile_name) or die ("cannot open $datafile_name to read.");
                while(!eof(FP)) {
                    $char = getc FP;
                    # 文字をアスキーコードに変換
                    $char = unpack("C", $char);
                    # アスキーコードを16進に変換
                    $char = sprintf("%x", $char);
                    $filedata .= '\x' . $char;
                }
                print "    $class_context( \"$filedata\" ),\n";
                print   FO  "    $class_context( \"$filedata\" ),\n";
                # ファイル読み込みフラグの初期化
                $datafile_name = undef;
                $filedata = "";
            }
            else{
                my @ascii;
                my $hexdata;
                # 文字をアスキーコードに変換
                @ascii = unpack("C*", $record->{$context});
                foreach my $i(@ascii){
                    # アスキーコードを16進に変換
                    $hexdata .= '\x' . sprintf("%x", $i);
                }
                print "    $class_context( \"$hexdata\" ),\n";
                print   FO  "    $class_context( \"$hexdata\" ),\n";
                # 毎回テキストを初期化する。
                $record->{$context} = "";
            }
        }
        elsif($name =~ /string/i){
            $record->{$context} =~ s/(["\\])/\\$1/g;
            $record->{$context} =~ s/[\n]/\\n/g;
            $record->{$context} =~ s/[\t]/\\t/g;
            print "    $class_context( \"$record->{$context}\" ),\n";
            print   FO  "    $class_context( \"$record->{$context}\" ),\n";
            # 毎回テキストを初期化する。
            $record->{$context} = "";
        }
        else{
            # ループでdefault_typeに当てはまるモノを探して処理
            foreach my $i(keys %default_type){
                if($name =~ /$i/i){
                    print "    $class_context( $record->{$context} ),\n";
                    print   FO  "    $class_context( $record->{$context} ),\n";
                    # 毎回テキストを初期化する。
                    $record->{$context} = "";
                }
            }
        }
        # テキストの初期化
        $class_context = "";
        $context = "";
    }
}

# 最後にファイルの終了を出力する。
sub handle_doc_end{
    print "ENVResource* resourceArray\[\]={";
    print   FO  "ENVResource* resourceArray\[\]={";
    print "$resource_names";
    print   FO  "$resource_names";
    print " NULL };";
    print   FO  " NULL };";
}

