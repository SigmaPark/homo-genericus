/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_Specification.hpp"


static void intro()
{
	sgm::spec::mdo
	<<	sgm::spec::Title("Introduction")
	<<	"This library helps you make manual documents for a code library "
	<<	"with its example codes." << sgm::spec::newl
	<<	"You can easily create the documents as Markdown format just by writing example codes."
	<<	sgm::spec::empty_line;
}


static void Math_Expression()
{
	sgm::spec::mdo 
	<<	sgm::spec::Title("Math Expression")
	<<	"Markdown supports writing mathematical expressions using LaTeX syntex." 
	<<	sgm::spec::newl
	<<	sgm::spec::Load_code_block("math_expression_ex") << sgm::spec::newl;

BEGIN_CODE_BLOCK(math_expression_ex)
	sgm::spec::mdo << R"($$ \sum_{n = 1}^{\infty}{n^{-2}} = \frac{\pi^{2}}{6} $$)";
END_CODE_BLOCK(math_expression_ex)

	sgm::spec::mdo << sgm::spec::empty_line;
}


static void Code_Block()
{
	sgm::spec::mdo
	<<	sgm::spec::Title("Code Block")
	<<	"Macro \"BEGIN_CODE_BLOCK\" and \"END_CODE_BLOCK\" captures codes." << sgm::spec::newl
	<<	sgm::spec::newl;

BEGIN_CODE_BLOCK(code_block_example_show)

BEGIN_CODE_BLOCK(code_block_ex)
	int sum = 0;

	for(int i = 1;  i <= 10;  ++i)
		sum += i;

	sgm::spec::is_True(sum == 55);
END_CODE_BLOCK(code_block_ex)

END_CODE_BLOCK(code_block_example_show)

	sgm::spec::mdo << sgm::spec::Load_code_block("code_block_example_show") << sgm::spec::newl;

	sgm::spec::mdo << sgm::spec::empty_line;
}


static void Assertion()
{
	sgm::spec::mdo
	<<	sgm::spec::Title("sgm::spec::is_True")
	<<	"You can not only show codes but also assert the result from the code is correct using "
	<<	"\"sgm::spec::is_True\"" << sgm::spec::newl;

BEGIN_CODE_BLOCK(is_true_ex)
	int result = 1 + 1;

	sgm::spec::is_True(result == 2);
END_CODE_BLOCK_AND_LOAD(is_true_ex)

	sgm::spec::mdo
	<<	"\"sgm::spec::is_True\" throws an sgm::Exception if the boolean parameter is false, "
	<<	"and the exception prevents making Markdown file export. " 
	<<	"Therefore the code result is correct "
	<<	"if the document file is successfully created."
	<<	sgm::spec::empty_line;
}


static void External_Resources()
{
	sgm::spec::mdo
	<<	sgm::spec::Title("External Resources")
	<<	"All external materials you want to attach to a document are to be located at "
	<<	"\"\\md_materials\"" << sgm::spec::empty_line;

	sgm::spec::mdo
	<<	sgm::spec::Title("Text File", 2)
	<<	"Use a macro function \"LOAD_DESCRIPTION_FILE\"" << sgm::spec::newl
	<<	sgm::spec::Load_code_block("descrip_file_ex") << sgm::spec::newl;


	{
		sgm::spec::md_block_guard mbg;

BEGIN_CODE_BLOCK(descrip_file_ex)
		sgm::spec::mdo << sgm::spec::Load_description_file("YOLO.txt");
END_CODE_BLOCK(descrip_file_ex)
	}

	sgm::spec::mdo << sgm::spec::empty_line;
	
	sgm::spec::mdo
	<<	sgm::spec::Title("Image File", 2)
	<<	"Use \"sgm::spec::Load_image\" function." << sgm::spec::newl
	<<	sgm::spec::Load_code_block("img_ex") << sgm::spec::newl;


BEGIN_CODE_BLOCK(img_ex)
	{
		sgm::spec::html_block_guard hbg("center");

		sgm::spec::mdo << sgm::spec::Load_image("sample_image.jpg", 720);
	}
END_CODE_BLOCK(img_ex)

	sgm::spec::mdo << sgm::spec::empty_line;

	sgm::spec::mdo
	<<	"The 2nd parameter means image width to show " 
	<<	"and can be omitted(then it shows the image by original size)."
	<<	sgm::spec::empty_line;
}


static void Guards()
{
	sgm::spec::mdo
	<<	sgm::spec::Title("Guards")
	<<	sgm::spec::Title("Block Guard", 2)
	<<	sgm::spec::Load_code_block("md_guard_ex") << sgm::spec::newl;

BEGIN_CODE_BLOCK(md_guard_ex)
	{
		sgm::spec::md_block_guard mbg;

		sgm::spec::mdo 
		<<	"Until \"mbg\" is alive(before its destructor is excuted), "
		<<	"all contents are in box tab." << sgm::spec::newl;
	}
END_CODE_BLOCK(md_guard_ex)


	sgm::spec::mdo
	<<	sgm::spec::newl
	<<	sgm::spec::Title("HTML Guard", 2)
	<<	sgm::spec::Load_code_block("html_guard_ex") << sgm::spec::newl;

BEGIN_CODE_BLOCK(html_guard_ex)
	{
		sgm::spec::html_block_guard hbg("center strong blockquote");
		
		sgm::spec::mdo 
		<<	"Until \"hbg\" is alive, "
		<<	"all contents are aligned at center, emphasized and quoted in block." 
		<<	sgm::spec::newl;
	}
END_CODE_BLOCK(html_guard_ex)

	sgm::spec::mdo << sgm::spec::empty_line;
	
	sgm::spec::mdo 
	<<	"Another way to apply HTML tags is to use \"sgm::spec::HTML_tag\" function."
	<<	sgm::spec::newl
	<<	sgm::spec::Load_code_block("html_tag_ex") << sgm::spec::newl;

BEGIN_CODE_BLOCK(html_tag_ex)
	sgm::spec::mdo 
	<<	sgm::spec::HTML_tag("HTML tags to a short messages.", "center strong blockquote");
END_CODE_BLOCK(html_tag_ex)

	
	sgm::spec::mdo << sgm::spec::empty_line;
}


static void Literal_Suffixes()
{
	sgm::spec::mdo
	<<	sgm::spec::Title("Literal Suffixes")
	<<	sgm::spec::Title("\"\"_mdo for Tabless Description", 2)
	<<	sgm::spec::Load_code_block("mdo_suffix_ex") << sgm::spec::newl;

BEGIN_CODE_BLOCK(mdo_suffix_ex)
	sgm::spec::mdo
	<<	R"(
			If you want to write something verbose, 
			pre-defined literal operator ""_mdo should be helpful.

			As if you write a normal script or comment, 
			describe what you want to explain freely.
					All blank spaces and tabs on left will be removed.
		)"_mdo;
END_CODE_BLOCK(mdo_suffix_ex)

	sgm::spec::mdo << sgm::spec::empty_line;


	sgm::spec::mdo
	<<	sgm::spec::Title("\"\"_code for Pseudo Code", 2)
	<<	"Sometimes, it can be effective to use peudo code which is not excutable "
	<<	"but easy to understand when you explain a logic and how it works. "
	<<	"A pre-defined literal operator \"\"_code helps you write the pseudo code very easily."
	<<	sgm::spec::newl
	<<	sgm::spec::Load_code_block("pseudo_code_ex") << sgm::spec::newl;

BEGIN_CODE_BLOCK(pseudo_code_ex)
	sgm::spec::mdo 
	<<	R"(
			Selection_Sort(A[], n)
			    for last <- downto 2
			        Find the greatest element A[k] among A[1...last]
			        Swap A[k] and A[last]
		)"_code;
END_CODE_BLOCK(pseudo_code_ex)

	sgm::spec::mdo << sgm::spec::empty_line;
}


static void outro()
{
	sgm::spec::mdo 
	<<	"----" << sgm::spec::newl
	<<	sgm::spec::HTML_tag("Thank you for watching", "strong center") << sgm::spec::newl;
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Specification, /**/)
{	::intro
,	::Math_Expression
,	::Code_Block
,	::Assertion
,	::External_Resources
,	::Guards
,	::Literal_Suffixes
,	::outro
};