#!/bin/ruby
#  k.ohno    2007.12.7 create
#  crcinsert.rb
#
#  使用方法：ruby crcinsert.rb list.txt
#  crc検査関数を差し込む関数をテーブルから読み込み埋め込んでいく
#  そのばかぎりのrubyスクリプト


$FILE_TAG = "File"
$LABEL_TAG = "Label"
$SETFUNC_TAG = "Set"
$CHECKFUNC_TAG = "Chk"


## 関数を埋め込むリスト
class FuncList
  def initialize
    @bSet = false
    @FuncParam = ""
    @FuncString2 = ""
    @SaveLabel = ""       ## GMDATA_ID_WIFILISTとか
    @SwitchTag = ""  ## 検索を行うのかどうか
  end
  
  def setBSet(bSet)
    @bSet =bSet
  end
  
  def setFuncParam( funcparam )
    @FuncParam = funcparam
  end
  
  def setSaveLabel( savelabel )
    @SaveLabel = savelabel
    @SwitchTag = "CRCLOADCHECK_" + @SaveLabel
  end
  
  def setFuncString( funcstring )
    @FuncString2 = funcstring.chomp
  end
  def getFuncString
    return @FuncString2
  end
  
  def getPripro
    return "#if (CRC_LOADCHECK && " + @SwitchTag + ")"
  end
  
  def getInsertFunc
    if @bSet == true
      return "\tSVLD_SetCrc(#{@SaveLabel});"
    else
      return "\tSVLD_CheckCrc(#{@SaveLabel});"
    end
  end
  
end

## ファイル変換情報を集めたクラス
class ConvFile
  def initialize
    @FILE_NAME = ""       ## /src/savedata/wifilist.cなど
    @SaveLabel = ""       ## GMDATA_ID_WIFILISTとか
    @SetListArray = []    ## 
  end
  def setFileName(fname)
    @FILE_NAME = fname
  end
  def getFileName
    return @FILE_NAME
  end
  
  def setSaveLabel(funcname)
    @SaveLabel = funcname
  end
  
  def addSetFuncListArray(bSet, line)
    fs = FuncList::new
    fs.setFuncString(line)
    fs.setBSet(bSet)
    fs.setSaveLabel(@SaveLabel)
    
    @SetListArray.push(fs)
  end
  def getSetListArray
    return @SetListArray
  end
    
end



##
##  入力テーブルをarrayに差し替える
##
def readTable( listArray, inputListFile )
  cf = ConvFile.new

  File.open(inputListFile){ |_file|
    _file.each{ |line|
      
      if /^#{$FILE_TAG}\s(.*)/ =~ line then
        if cf.getFileName != "" then
          listArray.push(cf)
        end
        cf = ConvFile.new
        cf.setFileName( $1 )
      elsif /^#{$SETFUNC_TAG}\s(.*)/ =~ line then
        cf.addSetFuncListArray(true,$1)
      elsif /^#{$CHECKFUNC_TAG}\s(.*)/ =~ line then
        cf.addSetFuncListArray(false,$1)
      elsif /^#{$LABEL_TAG}\s(.*)/ =~ line then
        cf.setSaveLabel( $1.chomp )
      end
    }
  }
  if cf.getFileName != "" then
    listArray.push(cf)
  end
end



##
##  テーブルファイルから関数があるかどうかを探す
##
##line  ファイルの一行
##
# $SETFUNC_FIND = 1
# $CHECKFUNC_FIND = 2


def scanFunc( line, convfileP )
  lineName = line.chomp
  
  convfileP.getSetListArray.each{ |flist|
    funcName = flist.getFuncString
    if funcName == lineName
      p funcName
      return flist
    end
  }
  return nil
end


def outFilePuts(outFile , fList)
  outFile.puts(fList.getPripro)
  outFile.puts(fList.getInsertFunc)
  outFile.puts("#endif //CRC_LOADCHECK")
end


##
##  定義にあった関数を見つけて定義を追加していく
##
def convertFile( readFile , writeFile, convfileP )
  r = `pwd`
  r = r.chomp
  setFuncFound = nil
  
  outFile = open(r + writeFile,"w")
  File.open(r + readFile){ |_file|
    _file.each{ |line|
      
      ret = scanFunc(line,convfileP)
      if ret != nil
        setFuncFound = ret
      end
      if /^\treturn.*/ =~ line
        if setFuncFound != nil
          outFilePuts(outFile, setFuncFound)
          setFuncFound = nil
        end
      elsif /^    return.*/ =~ line
        if setFuncFound != nil
          outFilePuts(outFile, setFuncFound)
          setFuncFound = nil
        end
      elsif /^\}/ =~ line
        if setFuncFound != nil
          outFilePuts(outFile, setFuncFound)
          setFuncFound = nil
        end
      end
      outFile.puts(line)
      }
  }
  outFile.close;
end



##main部分

InputTable = ARGV.shift    ## テーブルファイル読み込み
list_array = []

#解析を行う
readTable( list_array, InputTable )

list_array.each{ |cf|
  name = cf.getFileName.chomp
  convertFile(name, name+"new", cf)
  name = "." + name
  disp = `rm #{name}`
  p disp
  namecomm = name + "new " + name
  disp = `mv #{namecomm}`
  p disp
}


