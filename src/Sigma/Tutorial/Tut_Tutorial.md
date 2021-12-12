# Introduction
Tutorial library helps you make tutorial documents for a code library with its example codes.  
You can easily create the tutorial documents as Markdown format just by writing example codes.

&nbsp;  

# Math Expression
Markdown supports writing mathematical expressions using LaTeX syntex.  
```cpp
sgm::tut::mdo << R"($$ \sum_{n = 1}^{\infty}{n^{-2}} = \frac{\pi^{2}}{6} $$)";  
```
  
$$ \sum_{n = 1}^{\infty}{n^{-2}} = \frac{\pi^{2}}{6} $$

&nbsp;  

# Code Block
Macro "BEGIN_CODE_BLOCK" and "END_CODE_BLOCK" captures codes.  
<img src=".\md_materials\code_block_capture.png">  
```cpp
int sum = 0;  
  
for(int i = 1;  i <= 10;  ++i)  
	sum += i;  
  
sgm::tut::is_True(sum == 55);  
```
  


&nbsp;  

# sgm::tut::is_True
You can not only show codes but also assert the result from the code is correct using "sgm::tut::is_True"  
```cpp
int result = 1 + 1;  
  
sgm::tut::is_True(result == 2);  
```
"sgm::tut::is_True" throws an std::exception if the boolean parameter is false, and the exception prevents making Markdown file export. Therefore the code result is correct if the document file is successfully created.

&nbsp;  

# External Resources
All external materials you want to attach to a document are to be located at "\md_materials"  

## Text File
Use a macro function "LOAD_DESCRIPTION_FILE"  
```cpp
sgm::tut::mdo << LOAD_DESCRIPTION_FILE("YOLO.txt");  
```
  
```
You  
Only  
Live  
Once.  
  
Please never waste your time.  
```
  
## Image File
Use "sgm::tut::Load_image" function.  
```cpp
{  
	sgm::tut::html_block_guard hbg("center");  
  
	sgm::tut::mdo << sgm::tut::Load_image("sample_image.jpg", 720);  
}  
```
  
<center><img src=".\md_materials\sample_image.jpg" width ="720"></center>  
The 2nd parameter means image width to show and can be omitted(then it shows the image by original size).

&nbsp;  

# Guards
## Block Guard
```cpp
{  
	sgm::tut::md_block_guard mbg;  
  
	sgm::tut::mdo   
	<<	"Until \"mbg\" is alive(before its destructor is excuted), all contents are in box tab.  \n";  
}  
```
  
```
Until "mbg" is alive(before its destructor is excuted), all contents are in box tab.  
```
  
## HTML Guard
```cpp
{  
	sgm::tut::html_block_guard hbg("center strong blockquote");  
		  
	sgm::tut::mdo   
	<<	"Until \"hbg\" is alive, all contents are aligned at center, emphasized and quoted in block.  \n";  
}  
```
  
<center><strong><blockquote>Until "hbg" is alive, all contents are aligned at center, emphasized and quoted in block.  
</center></strong></blockquote>

&nbsp;  

Another way to apply HTML tags to a short text is to use "sgm::tut::HTML_tag" function.

```cpp
sgm::tut::mdo   
<<	sgm::tut::HTML_tag("HTML tags to a short messages.  \n", "center strong blockquote");  
```
  
<blockquote><strong><center>HTML tags to a short messages.  
</center></strong></blockquote>

&nbsp;  

# Literal Suffixes
##  ""_mdo for Tabless Description
```cpp
sgm::tut::mdo  
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
sgm::tut::mdo   
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
