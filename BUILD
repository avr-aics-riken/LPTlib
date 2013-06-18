                           LPT libraryのビルド手順


0. 事前準備
   LPTの性能情報をPMlibを用いて測定する場合は、事前にPMlibをコンパイルしておく必要があります。


1. パスの設定
   Mkinclude.LPTを編集し、以下の変数を設定します。
     PMLIB_DIR: PMlibのインストール先ディレクトリ
     LPT_DIR:   LPTのアーカイブを展開したディレクトリ(このファイルが存在するディレクトリ)


2. Buildに使用するコンパイラ, MPIライブラリ等の設定
   Mkinclude.LPT内でインクルードしているファイルを変更し、
   コンパイラ名, コンパイルオプション等を指定します。
   本アーカイブ内には、参考として以下の3種類の環境向けの設定ファイルを同梱しています。 
      Mkinclude.GNU_OpenMPI     gcc version 4.4 + OpenMPI  (CentOS 5.5)
      Mkinclude.Intel_IntelMPI  Intel Cluster Toolkit version 12.
      Mkinclude.Intel_OpenMPI   Inte C++ Compiler ver. 12 + OpenMPI

   これらのファイル内ではFortranおよびCコンパイラに関しても設定していますが
   本ライブラリのビルド時にはC++コンパイラしか使用していません。


3. ライブラリおよびファイルコンバータのbuild
   以下のコマンドを実行すると、libディレクトリ内にLPTライブラリ(libLPT.a)が、
   binディレクトリ内にファイルコンバータの実行ファイル(FileConverter)が生成されます。

     >make clean; make depend;make


