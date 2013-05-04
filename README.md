php-ext-trie-filter
===================

php extension for spam word filter based on Double-Array Trie tree, it can detect if a spam word exists in a text message.

关键词过滤扩展，用于检查一段文本中是否出现敏感词，基于Double-Array Trie 树实现。

## 升级历史

### 2013-05-04
1.	每个word可以附加一个int32类型的私有值，在trie_filter_search/trie_filter_search_all的返回值中，该私有值和word一起返回
2.	新增trie_filter_search_all，一次返回所有的命中词，注意第二个词是从第一个词之后开始查找。

## 依赖库

[libdatrie-0.2.4 or later](http://linux.thai.net/~thep/datrie/datrie.html)

## 安装步骤

下面的$LIB_PATH为依赖库安装目录，$INSTALL_PHP_PATH为PHP5安装目录。

### 安装libdatrie
    $ tar zxvf libdatrie-0.2.4.tar.gz
    $ cd libdatrie-0.2.4
    $ make clean
    $ ./configure --prefix=$LIB_PATH
    $ make
    $ make install

### 安装扩展   
    $ $INSTALL_PHP_PATH/bin/phpize
    $ ./configure --with-php-config=$INSTALL_PHP_PATH/bin/php-config --with-trie_filter=$LIB_PATH
    $ make
    $ make install

然后修改php.ini，增加一行：extension=trie_filter.so，然后重启PHP。

## 使用示例
	<?php
	$arrWord = array('word1', 'word2', 'word3');
	$resTrie = trie_filter_new(); //create an empty trie tree
	foreach ($arrWord as $k => $v) {
    	trie_filter_store($resTrie, $v, $k + 1);
	}
	trie_filter_save($resTrie, __DIR__ . '/blackword.tree');

	$resTrie = trie_filter_load(__DIR__ . '/blackword.tree');

	$strContent = 'hello word2 word1';
	$arrRet = trie_filter_search($resTrie, $strContent);
	print_r($arrRet); //Array(0 => 6, 1 => 5, 2 => 2)
	echo substr($strContent, $arrRet[0], $arrRet[1]); //word2
	$arrRet = trie_filter_search_all($resTrie, $strContent);
	print_r($arrRet); //Array(0 => Array(0 => 6, 1 => 5, 2 => 2), 1 => Array(0 => 12, 1 => 5, 2 => 1))
	

	$arrRet = trie_filter_search($resTrie, 'hello word');
	print_r($arrRet); //Array()

	trie_filter_free($resTrie);

# PHP版本

PHP 5.2 or later.

Windows is not support until now.

## License

Apache License 2.0

## 致谢

本项目是在[用于检测敏感词的 PHP 扩展](http://blog.anbutu.com/php/php-ext-trie-filter)的基础上改写的。

