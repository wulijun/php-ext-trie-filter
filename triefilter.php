<?php
/**
 * Helper autocomplete for php trie-filter extension
 * @author wulijun01234 <wulijun01234@gmail.com>
 * @link https://github.com/wulijun/php-ext-trie-filter
 */

/**
 * Load a trie tree from a saved trie tree file
 * 
 * @param string $strDictFile Path to saved trie tree file
 * @return resource The trie tree handler or NULL on error.
 */
function trie_filter_load($strDictFile) {
	
}

/**
 * Find if a spam word exists in the content
 * 
 * @param resource $resTree
 * @param string $strContent
 * @return array spam word info, like Array(0 => start position, 1 => spam word len), if no one found, return empty array, false on error.
 * @example
 * <pre>
 * $arrSpamWord = trie_filter_search($resTree, $strContent);
 * if (! empty($arrSpamWord)) {
 *     echo substr($strContent, $arrSpamWord[0], $arrSpamWord[1]);
 * }
 * </pre>
 */
function trie_filter_search($resTree, $strContent) {
	
}

/**
 * Find all spam word exists in the content
 * 
 * @param resource $resTree
 * @param string $strContent
 * @return array spam word info, like Array(Array(0 => start position, 1 => spam word len)), if no one found, return empty array, false on error.
 * @example
 * <pre>
 * $arrSpamWord = trie_filter_search_all($resTree, $strContent);
 * if (! empty($arrSpamWord)) {
 *     foreach ($arrSpamWord as $arrOneWord) {
 *         echo substr($strContent, $arrOneWord[0], $arrOneWord[1]);
 *     }
 * }
 * </pre>
 */
function trie_filter_search_all($resTree, $strContent) {
	
}

/**
 * Create an empty trie tree
 *
 * @return resource The trie tree handler or NULL on error.
 */
function trie_filter_new() {
	
}

/**
 * Add a word to the trie tree
 * 
 * @param resource $resTree
 * @param string $strWord
 * @return bool true on success or false on error.
 */
function trie_filter_store($resTree, $strWord) {

}

/* {{{ proto array (int trie_tree_identifier, string dict_path)
 Returns true, or false on error*/

/**
 * Save trie tree to a file
 *
 * @param resource $resTree
 * @param string $strDictFile
 * @return bool true on success or false on error.
 */
function trie_filter_save($resTree, $strDictFile) {

}

/**
 * Free trie tree
 * 
 * Trie tree will be destructed automaticly when script finished, however, you can free it yourself. 
 *
 * @param resource $resTree
 * @return bool true on success or false on error.
 */
function trie_filter_free($resTree) {

}
