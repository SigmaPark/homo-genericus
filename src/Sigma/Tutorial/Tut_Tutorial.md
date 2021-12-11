# Introduction
Tutorial library helps you make tutorial documents for a code library with its example codes.  
You can easily create the tutorial documents as Markdown format just by writing example codes.

&nbsp;  

# Math Expression
Markdown supports writing mathematical expressions using LaTeX syntex.  
$$ \sum_{n = 1}^{\infty}{n^{-2}} = \frac{\pi^{2}}{6} $$

&nbsp;  

# Code Block
Macro "BEGIN_CODE_BLOCK" and "END_CODE_BLOCK" captures codes.  
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
  
	sgm::tut::mdo << sgm::tut::Load_image("sample_img.png", 200);  
}  
```
  
<center><img src=".\md_materials\sample_img.png" width ="200"></center>  
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
<strong><center>Thank you for watching</strong></center>