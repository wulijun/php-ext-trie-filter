<?php
ini_set('display_errors', 1);
$arrWord = array('word', 'word1', 'word3', 'a', 'ab', 'abc', 'b');
$resTrie = trie_filter_new(); //create an empty trie tree
foreach ($arrWord as $k => $v) {
        trie_filter_store($resTrie, $v);
}
trie_filter_save($resTrie, __DIR__ . '/blackword.tree');

$resTrie = trie_filter_load(__DIR__ . '/blackword.tree');
$str='hello word2 haha word1 word4 word2';
$arrRet = trie_filter_search($resTrie, $str);
print_all($str,array($arrRet)); //Array(0 => 6, 1 => 5)
echo "\ntest1///////////////////\n";
$str = 'hello word2 haha word1 word4 word2';
$arrRet = trie_filter_search_all($resTrie, $str);
print_all($str, $arrRet);
echo "\ntest2///////////////////\n";
$str = 'hello word';
$arrRet = trie_filter_search($resTrie, $str);
print_all($str, array($arrRet)); //Array()
$arrRet = trie_filter_search_all($resTrie, 'hello word');
print_all($str, $arrRet);

echo "\ntest3///////////////////\n";
echo "start memory=".memory_get_usage(true)."\n";date_default_timezone_set('Asia/Chongqing');
$test = array('a', 'abd', 'dad', 'pab', 'dda', 'word1f', 'cword1', 'cword1t');
foreach ($test as $v) {
//    echo "per start memory=".memory_get_usage(true)."\n";
    $arrRet = trie_filter_search_all($resTrie, $v);
  //  echo "per end memory=".memory_get_usage(true)."\n";
    //print_all($v, $arrRet);
}
echo "end memory=".memory_get_usage(true)."\n";
echo date('Y-m-d H:i:s');
trie_filter_free($resTrie);
function print_all($str, $res) {//print_r($res);
    echo "$str\n";
    foreach ($res as $k => $v) {
        echo $k."=>{$v[0]}-{$v[1]}-".substr($str, $v[0], $v[1])."\n";
    }
}

