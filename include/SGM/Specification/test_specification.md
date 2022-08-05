# Introduction
This library helps you make manual documents for a code library with its example codes.  
You can easily create the documents as Markdown format just by writing example codes.

&nbsp;  

# Math Expression
Markdown supports writing mathematical expressions using LaTeX syntex.  
```cpp
sgm::spec::mdo << R"($$ \sum_{n = 1}^{\infty}{n^{-2}} = \frac{\pi^{2}}{6} $$)";  
```
  
$$ \sum_{n = 1}^{\infty}{n^{-2}} = \frac{\pi^{2}}{6} $$

&nbsp;  

# Code Block
Macro "BEGIN_CODE_BLOCK" and "END_CODE_BLOCK" captures codes.  
  
```cpp
BEGIN_CODE_BLOCK(code_block_ex)  
	int sum = 0;  
  
	for(int i = 1;  i <= 10;  ++i)  
		sum += i;  
  
	sgm::spec::is_True(sum == 55);  
END_CODE_BLOCK(code_block_ex)  
  
```
  


&nbsp;  

# sgm::spec::is_True
You can not only show codes but also assert the result from the code is correct using "sgm::spec::is_True"  
```cpp
int result = 1 + 1;  
  
sgm::spec::is_True(result == 2);  
```
"sgm::spec::is_True" throws an std::exception if the boolean parameter is false, and the exception prevents making Markdown file export. Therefore the code result is correct if the document file is successfully created.

&nbsp;  

# External Resources
All external materials you want to attach to a document are to be located at "\md_materials"

&nbsp;  

## Text File
Use a macro function "LOAD_DESCRIPTION_FILE"  
```cpp
sgm::spec::mdo << sgm::spec::Load_description_file("YOLO.txt");  
```
  
```
You  
Only  
Live  
Once.  
  
Please never waste your time.  
```


&nbsp;  

## Image File
Use "sgm::spec::Load_image" function.  
```cpp
{  
	sgm::spec::html_block_guard hbg("center");  
  
	sgm::spec::mdo << sgm::spec::Load_image("sample_image.jpg", 720);  
}  
```
  
<center><img src="./md_materials/sample_image.jpg" width ="720"></center>

&nbsp;  

The 2nd parameter means image width to show and can be omitted(then it shows the image by original size).

&nbsp;  

# Guards
## Block Guard
```cpp
{  
	sgm::spec::md_block_guard mbg;  
  
	sgm::spec::mdo   
	<<	"Until \"mbg\" is alive(before its destructor is excuted), "  
	<<	"all contents are in box tab." << sgm::spec::newl;  
}  
```
  
```
Until "mbg" is alive(before its destructor is excuted), all contents are in box tab.  
```
  
## HTML Guard
```cpp
{  
	sgm::spec::html_block_guard hbg("center strong blockquote");  
		  
	sgm::spec::mdo   
	<<	"Until \"hbg\" is alive, "  
	<<	"all contents are aligned at center, emphasized and quoted in block."   
	<<	sgm::spec::newl;  
}  
```
  
<center><strong><blockquote>Until "hbg" is alive, all contents are aligned at center, emphasized and quoted in block.  
</center></strong></blockquote>

&nbsp;  

Another way to apply HTML tags is to use "sgm::spec::HTML_tag" function.  
```cpp
sgm::spec::mdo   
<<	sgm::spec::HTML_tag("HTML tags to a short messages.", "center strong blockquote");  
```
  
<blockquote><strong><center>HTML tags to a short messages.</center></strong></blockquote>

&nbsp;  

# Literal Suffixes
## ""_mdo for Tabless Description
```cpp
sgm::spec::mdo  
<<	R"(  
		If you want to write something verbose,   
		pre-defined literal operator ""_mdo should be helpful.  
  
		As if you write a normal script or comment,   
		describe what you want to explain freely.  
				All blank spaces and tabs on left will be removed.  
	)"_mdo;  
```
  
If you want to write something verbose,   
pre-defined literal operator ""_mdo should be helpful.  
  
As if you write a normal script or comment,   
describe what you want to explain freely.  
All blank spaces and tabs on left will be removed.  
		  


&nbsp;  

## ""_code for Pseudo Code
Sometimes, it can be effective to use peudo code which is not excutable but easy to understand when you explain a logic and how it works. A pre-defined literal operator ""_code helps you write the pseudo code very easily.  
```cpp
sgm::spec::mdo   
<<	R"(  
		Selection_Sort(A[], n)  
		    for last <- downto 2  
		        Find the greatest element A[k] among A[1...last]  
		        Swap A[k] and A[last]  
	)"_code;  
```
  
```
Selection_Sort(A[], n)  
    for last <- downto 2  
        Find the greatest element A[k] among A[1...last]  
        Swap A[k] and A[last]  
```


&nbsp;  

----  
<center><strong>Thank you for watching</strong></center>  
