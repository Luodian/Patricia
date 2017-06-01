//
//  patricia.hpp
//  Patricia
//
//  Created by 李博 on 24/05/2017.
//  Copyright © 2017 李博. All rights reserved.
//

#ifndef patricia_h
#define patricia_h

#include "Header.hpp"

namespace Radix
{
	template<class K,class V>
	struct patricia_node
	{
		K key;
		V value;
		std::map<char,patricia_node<K,V> *> childrens;
		bool terminal;
		patricia_node(K _key = K(), V _value = V(), bool _terminal = 1)
		{
			key = _key;
			value = _value;
			terminal = _terminal;
			childrens.clear();
		}
	};
	template<class K,class V>
	class patricia_tree
	{
	public:
		patricia_tree()
		{
			root = nullptr;
			node_count = 0;
			word_count = 0;
		}
		void insert(K key, V value)
		{
			word_count += key.size();
			if (key.size() == 0)
			{
				return;
			}
			else if (root == nullptr)
			{
				root = new patricia_node<K,V>;
			}
			insert(key,value,root);
		}
		void remove(K key, V value)
		{
			if (key.size() == 0 || root == nullptr)
			{
				return;
			}
			else 
			{
				remove(key,value,root);
			}
		}
		V find(K key)
		{
			if (key.size() == 0 || root == nullptr)
			{
				throw "can not find such key.";
			}
			else
			{
				if (find(key,root) == V())
				{
					throw "can not find such key.";
				}
				else
				{
					return find(key,root);
				}
			}
		}

		size_t space_size()
		{
			//return byte num;
			return (4 * 8 + 1 + 1 + 8 * 8) * node_count + word_count * 1 * 8;
		}
		
		std::vector<std::string> prefix_search(const std::string &prefix)
		{
			std::vector<std::string> ret;
			prefix_search(prefix,root,ret);
			return ret;
		}
		// void f()
		// {
		// 	std::string a = "aaaa";
		// 	std::string b = "aaaaab";
		// 	std::cout<<substring(b,a)<<"\n";
		// }
	private:
		size_t node_count;
		size_t word_count;
		std::string substring(std::string long_string, std::string short_string)
		{
			std::string sub;
			sub.clear();
			for (int i = 0; i < long_string.size(); ++i)
			{			
				if (i >= short_string.size())
				{
					sub.push_back(long_string[i]);
				}
			}
			return sub;
		}
		void prefix_dfs(patricia_node<K,V> * & current_node,std::string &ans,std::vector<std::string> &ret)
		{
			ans += current_node->key;
			if (current_node->terminal == 1)
			{
				ret.push_back(ans);
			}
			for (auto itr : current_node->childrens)
			{
				prefix_dfs(itr.second,ans,ret);
			}
		}
		void prefix_search(const std::string prefix,patricia_node<K,V> * current_node,std::vector<std::string> &ret)
		{
			ret.clear();
            typename std::map<char,patricia_node<K,V> *> ::iterator itr = current_node->childrens.find(prefix[0]);

            if (itr != current_node->childrens.end())
            {
            	current_node = itr->second;
                std::string ans_prefix;
                ans_prefix.clear();
                int i,j;
            	int match_size = 0;
            	for (i = 0; i < prefix.size();)
            	{
                    ans_prefix += current_node->key;
            		size_t len = std::min(current_node->key.size(),prefix.size());
            		j = 0;
            		for (; j < len; ++j)
            		{
            			if (prefix[j] != itr->second->key[j])
            			{
            				break;
            			}
            		}
            		match_size += j;
            		if (match_size < prefix.size())
            		{
            			i += j;
            			current_node = current_node->childrens[prefix[i]];
            		}
            		else if (match_size >= prefix.size())
            		{
            			for (auto k : current_node->childrens)
            			{
                            std::string ans;
                            ans.clear();
                            ans += ans_prefix;
            				prefix_dfs(k.second,ans,ret);
            			}
                        if (current_node->terminal == 1)
                        {
                            ret.push_back(ans_prefix);
                        }
            			return;
            		}
            		// if (j <= current_node->key.size())
            		// {
            		// 	for (auto k : current_node->childrens)
            		// 	{
            		// 		ret.push_back(prefix_dfs(k.second));
            		// 	}
            		// 	return ret;
            		// }
            		// else
            		// {
            		// 	i += j;
            		// 	current_node = current_node->childrens[prefix[i]];
            		// }
            	}
            }
            else
            {
            	throw "can not find this prefix.\n";
            }
            return;
		}

		V find(K key, patricia_node<K,V> * &current_node)
		{
            typename std::map<char,patricia_node<K,V> *> ::iterator itr = current_node->childrens.find(key[0]);
			if (itr != current_node->childrens.end())
			{
				size_t len = std::min(itr->second->key.size(),key.size());
				int j = 0;
				for (;j < len; ++j)
				{
					if (key[j] != itr->second->key[j])
					{
						break;
					}
				}

				if (j == 0)
				{
					return V();
				}
				else
				{
					//current child's key partially matches with the new key; 0<j<=len
					if (j == len)
					{
						if (key.size() == itr->second->key.size())
						{
							if (itr->second->terminal)
							{
								return itr->second->value;
							}
							else
							{
								return V();
							}
						}
						else if (key.size() > itr->second->key.size())
						{
							std::string sub_key = key.substr(j,key.size());
							return find(sub_key,itr->second);
						}
						else
						{
							return V();
						}
					}
					else
					{
						return V();
					}
				}
			}
			return V();
		}
		void remove(K key,V value, patricia_node<K,V> * &current_node)
		{
            typename std::map<char,patricia_node<K,V> *> ::iterator itr = current_node->childrens.find(key[0]);
            //if can't match the first character.
            if ( itr != current_node->childrens.end() )
            {	
	            size_t len = std::min( key.size(),itr->second->key.size() );
	            int j = 0;
	            for (; j < len; ++j)
	            {
	            	if (key[j] != itr->second->key[j])
	            	{
	            		break;
	            	}
	            }	
	            //fully match.
	            if (j == len)
	            {
	            	if ( key.size() == itr->second->key.size() )
	            	{
	            		//if totally match
	            		if (itr->second->terminal)
	            		{
							//e.g. child="ab#", key="ab", currNode="a"
							//	     a                    
							//     /   \
							//   d       ab#
							if (itr->second->childrens.size() == 0)
							{
								if (current_node->childrens[key[0]]->value == value)
								{
                                	current_node->childrens.erase(key[0]);
								}
                               	else
                               	{
                               		std::cout<<"The value can not match the key.\n";
                               	}
								//merge node for currnode
								if (!current_node->terminal && current_node->childrens.size() == 1)
								{
									patricia_node<K,V> *single_child = current_node->childrens.begin()->second;
									current_node->key += single_child->key;
									current_node->value = single_child->value;
									current_node->terminal = single_child->terminal;
									current_node->childrens = single_child->childrens;
								}
							}
							else
							{
								//e.g. child="ab#", key="ab", currNode="a"
								//    a#
								//     /
								//	   ab#                    
								//       /    
								//        f#    
								itr->second->terminal = false;
								//child
								if (itr->second->childrens.size() == 1)
								{
									patricia_node<K,V> *single_child = itr->second->childrens.begin()->second;
									itr->second->key += single_child->key;
									itr->second->value = single_child->value;
									itr->second->terminal = single_child->terminal;
									itr->second->childrens = single_child->childrens;
								}
							}
	            		}
	            		else
	            		{
							//e.g. child="ab", key="ab"
							//	   ab                    
							//    /  /  
							//   e    f    
							std::cout<<"No such key to remove.\n";
	            		}
	            	}
	            	else if (key.size() > itr->second->key.size())
	            	{
						//e.g. child="ab#", key="abc"
						//	   ab#                     
						//    /  /     						
						//   a    c#   		
						std::string sub_key = key.substr(j,key.size());
						remove(sub_key,value,itr->second);
	            	}
	            	else
	            	{
	            		std::cout<<"No such key to remove.\n";
	            	}
	            }
	            else
	            {
	            	std::cout<<"No such key to remove.\n";
	            }	
	        }		
            else 
            {
            	std::cout<<"No such key to remove.\n";
            	return;
            }
		}
		void insert(K key, V value, patricia_node<K,V> * &current_node)
		{		
			bool done = false;
            typename std::map<char,patricia_node<K,V> *> ::iterator itr = current_node->childrens.find(key[0]);
			if ( itr != current_node->childrens.end() )
			{
				size_t len = std::min(key.size(),itr->second->key.size());
				int j = 0;
				for (;j < len; ++j)
				{
					if (key[j] != itr->second->key[j])
					{
						break;
					}
				}
				//e.g. child="e" (currNode="abc")
				//	   abc                     abc
				//    /  /    =========>      / | /
				//   e    f   insert "c"     c# e  f
				if (j == 0)
				{
					patricia_node<K,V>* new_node = new patricia_node<K,V>(key,value,1);
					node_count ++;
					current_node->childrens[key[0]] = new_node;
					done = true;
				}
				else
				{
					//totally match
					if (j == len)
					{
						if (key.size() == itr->second->key.size())
						{
							if (itr->second->terminal == 1)
							{
								// std::cout<<"Duplicate key is found.\n";
							}
							else
							{
								itr->second->terminal = 1;
								//itr->key = key;
							}
							done = 1;
						}
						//e.g. child="ab#"
						//	   ab#                    ab#
						//    /  /    ==========>    / | / 							
						//   e    f   insert "abc"  c# e  f		
						else if (key.size() > itr->second->key.size())
						{
                            std::string sub_str = substring(key,itr->second->key);
							insert(sub_str,value,itr->second);
							done = 1;
						}
						//e.g. child="abc#"
						//	   abc#                      ab#
						//    /   /      =========>      /   
						//   e     f     insert "ab"    c#    
						//					           /  /
						//                            e    f		
						else
						{
							std::string sub_str = substring(itr->second->key,key);
							patricia_node<K,V> *sub_child = new patricia_node<K,V>(sub_str,value);
							node_count ++;
							sub_child->terminal = itr->second->terminal;
                            sub_child->value = itr->second->value;
							sub_child->childrens = itr->second->childrens;

							itr->second->key = key;
							itr->second->terminal = 1;
							itr->second->value = value;

							itr->second->childrens.clear();
							itr->second->childrens[sub_str[0]] = sub_child;
                            done = 1;
						}
					}
					else if (j > 0 && j < len)
					{
						//0<j<len
						//e.g. child="abc#"
						//	   abc#                     ab
						//    /  /     ==========>     / /
						//   e    f   insert "abd"    c#  d# 
						//                           /  /
						//                          e    f					
						//split at j
						std::string child_key = itr->second->key.substr(j,itr->second->key.size()); //c
						std::string sub_key = key.substr(j,key.size()); //d
						patricia_node<K,V>* sub_child = new patricia_node<K,V>(child_key);
						node_count ++;

						sub_child->terminal = itr->second->terminal;
						sub_child->value = itr->second->value;//inherited father's value
						sub_child->childrens = itr->second->childrens;

						itr->second->childrens.clear();
                        itr->second->key = itr->second->key.substr(0,j);
						itr->second->childrens[child_key[0]] = sub_child;
						itr->second->terminal = false;

						patricia_node<K,V>* sub_another_child = new patricia_node<K,V>(sub_key);
						node_count ++;

						sub_another_child->terminal = 1;
                        sub_another_child->value = value;
                        itr->second->childrens[sub_key[0]] = sub_another_child;
                        done = true;
					}
				}
			}
			if (done == false)
			{
				patricia_node<K,V>* new_node = new patricia_node<K,V>(key,value,1);
				node_count++;
				current_node->childrens[key[0]] = new_node;
			}
		}
		patricia_node<K,V> *root;
	};
}	


#endif /* patricia_h */
