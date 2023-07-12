/*  SPDX-FileCopyrightText: (c) 2021 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
*   SPDX-License-Identifier: MIT License
*/
//========//========//========//========//=======#//========//========//========//========//=======#


#include "Test_Specification.hpp"


static void intro()
{
	sgm::spec::mdo
	<<	sgm::spec::Title(L"Introduction")
	<<	L"This library helps you make manual documents for a code library "
	<<	L"with its example codes." << sgm::spec::newl
	<<	L"You can easily create the documents as Markdown format just by writing example codes."
	<<	sgm::spec::empty_line;
}


static void Math_Expression()
{
	sgm::spec::mdo 
	<<	sgm::spec::Title(L"Math Expression")
	<<	L"Markdown supports writing mathematical expressions using LaTeX syntex." 
	<<	sgm::spec::newl
	<<	sgm::spec::Load_code_block(L"math_expression_ex") << sgm::spec::newl;

BEGIN_CODE_BLOCK(math_expression_ex)
	sgm::spec::mdo << LR"($$ \sum_{n = 1}^{\infty}{n^{-2}} = \frac{\pi^{2}}{6} $$)";
END_CODE_BLOCK(math_expression_ex)

	sgm::spec::mdo << sgm::spec::empty_line;
}


static void Code_Block()
{
	sgm::spec::mdo
	<<	sgm::spec::Title(L"Code Block")
	<<	L"Macro \"BEGIN_CODE_BLOCK\" and \"END_CODE_BLOCK\" captures codes." << sgm::spec::newl
	<<	sgm::spec::newl;

BEGIN_CODE_BLOCK(code_block_example_show)

BEGIN_CODE_BLOCK(code_block_ex)
	int sum = 0;

	for(int i = 1;  i <= 10;  ++i)
		sum += i;

	sgm::spec::is_True(sum == 55);
END_CODE_BLOCK(code_block_ex)

END_CODE_BLOCK(code_block_example_show)

	sgm::spec::mdo << sgm::spec::Load_code_block(L"code_block_example_show") << sgm::spec::newl;

	sgm::spec::mdo << sgm::spec::empty_line;
}


static void Assertions()
{
	sgm::spec::mdo
	<<	sgm::spec::Title(L"Helper Functions for Assertion")
	<<	L"You can not only show codes but also assert the result from the code is correct "
	<<	L"using assertion helper functions. They check if the code works fine. When the code "
	<<	L"fails the assertion, sgm::Exception is thrown, log message will be shown, and "
	<<	L"Markdown document file won't be exported "
	<<	L"(if there was an old document already, it will be eliminated) . " 
	<<	L"Therefore it is guaranteed the code result is correct "
	<<	L"if the document file is successfully created."
	<<	sgm::spec::empty_line;


	sgm::spec::mdo
	<<	sgm::spec::Title(L"sgm::spec::is_True", 2)
	<<	L"\"sgm::spec::is_True\" checks an boolean expression is true." << sgm::spec::newl;

BEGIN_CODE_BLOCK(is_true_ex)
	int result = 1 + 1;

	sgm::spec::is_True(result == 2);
END_CODE_BLOCK_AND_LOAD(is_true_ex)

	sgm::spec::mdo << sgm::spec::empty_line;


	sgm::spec::mdo 
	<<	sgm::spec::Title(L"sgm::spec::Are_All_True / sgm::spec::Are_N_True", 2)
	<<	L"\"sgm::spec::Are_All_True\" checks whether all elements in a range on 1st parameter "
	<<	L"satisfy a condition function given as 2nd parameter. " 
	<<	L"\"sgm::spec::Are_N_True\" do the same thing on partial range from an iterator ." 
	<<	sgm::spec::newl;

BEGIN_CODE_BLOCK(are_all_true_ex)
	{
		std::initializer_list<int> range{2, 4, 6, 8, 10, 12};

		auto are_even_f = [](int n)-> bool{  return n % 2 == 0;  };
		auto are_less_than_10_f = [](int n)-> bool{  return n < 10;  };

		sgm::spec::Are_All_True(range, are_even_f);
		sgm::spec::Are_N_True(range.begin(), 4, are_less_than_10_f);
	}
END_CODE_BLOCK_AND_LOAD(are_all_true_ex)

	sgm::spec::mdo << sgm::spec::empty_line;


	sgm::spec::mdo 
	<<	sgm::spec::Title(L"sgm::spec::Are_All_Equivalent_to / sgm::spec::Are_N_Equivalent_to", 2)
	<<	L"\"sgm::spec::Are_All_Equivalent_to\" checks whether all elements in a range on 1st "
	<<	L"parameter are equivalent to 2nd parameter. You can specify the function that determines "
	<<	L"whether they are equivalent on 3rd parameter ( operator== as default ) . "
	<<	L"\"sgm::spec::Are_N_Equivalent_to\" do the same thing on partial range "
	<<	L"from an iterator . " << sgm::spec::newl;

BEGIN_CODE_BLOCK(are_all_equivalent_ex)
	{
		std::initializer_list<int> range{7, 7, 7, 7};

		sgm::spec::Are_All_Equivalent_to(range, 7);
	}
	{
		std::initializer_list<int> 
			range0{7, 7, 7, -7, -7},
			range1{3, 5, 7, 7, 7, -7, -7, -5, -3};

		auto abs_value_are_same_f 
		=	[](int n0, int n1)-> bool
			{
				int abs_n0 = n0 >= 0 ? n0 : -n0;
				int abs_n1 = n1 >= 0 ? n1 : -n1;

				return abs_n0 == abs_n1;
			};

		sgm::spec::Are_All_Equivalent_to(range0, 7, abs_value_are_same_f);

		sgm::spec::Are_N_Equivalent_to(range0.begin(), 3, 7);
		sgm::spec::Are_N_Equivalent_to(range1.begin() + 2, 3, 7);
		sgm::spec::Are_N_Equivalent_to(range1.begin() + 2, 5, 7, abs_value_are_same_f);
	}
END_CODE_BLOCK_AND_LOAD(are_all_equivalent_ex)

	sgm::spec::mdo << sgm::spec::empty_line;


	sgm::spec::mdo
	<<	sgm::spec::Title(L"sgm::spec::Are_Equivalent_Ranges", 2)
	<<	L"\"sgm::spec::Are_Equivalent_Ranges\" checks whether two ranges given as 1st and 2nd "
	<<	L"parameters are the same (the number of elements are same and have the same elements "
	<<	L"in the same order) . You can specify the function that determines whether the elements "
	<<	L"are same each others ( operator== as default ) . " << sgm::spec::newl;

BEGIN_CODE_BLOCK(are_equivalent_ranges_ex)
	{
		std::initializer_list<int> 
			range0{2, 4, 6, 8}, 
			range1{2, 4, 6, 8},
			range2{2, -4, 6, -8};

		auto abs_value_are_same_f 
		=	[](int n0, int n1)-> bool
			{
				int abs_n0 = n0 >= 0 ? n0 : -n0;
				int abs_n1 = n1 >= 0 ? n1 : -n1;

				return abs_n0 == abs_n1;
			};	

		sgm::spec::Are_Equivalent_Ranges(range0, range1);
		sgm::spec::Are_Equivalent_Ranges(range1, range2, abs_value_are_same_f);
	}
END_CODE_BLOCK_AND_LOAD(are_equivalent_ranges_ex)

	sgm::spec::mdo << sgm::spec::empty_line;	
}


static void External_Resources()
{
	sgm::spec::mdo
	<<	sgm::spec::Title(L"External Resources")
	<<	L"All external materials you want to attach to a document are to be located at "
	<<	L"\"\\md_materials\"" << sgm::spec::empty_line;

	sgm::spec::mdo
	<<	sgm::spec::Title(L"Text File", 2)
	<<	L"Use a macro function \"LOAD_DESCRIPTION_FILE\"" << sgm::spec::newl
	<<	sgm::spec::Load_code_block(L"descrip_file_ex") << sgm::spec::newl;


	{
		sgm::spec::md_block_guard mbg;

BEGIN_CODE_BLOCK(descrip_file_ex)
		sgm::spec::mdo << sgm::spec::Load_description_file(L"YOLO.txt");
END_CODE_BLOCK(descrip_file_ex)
	}

	sgm::spec::mdo << sgm::spec::empty_line;
	
	sgm::spec::mdo
	<<	sgm::spec::Title(L"Image File", 2)
	<<	L"Use \"sgm::spec::Load_image\" function." << sgm::spec::newl
	<<	sgm::spec::Load_code_block(L"img_ex") << sgm::spec::newl;


BEGIN_CODE_BLOCK(img_ex)
	{
		sgm::spec::html_block_guard hbg(L"center");

		sgm::spec::mdo << sgm::spec::Load_image(L"sample_image.jpg", 720);
	}
END_CODE_BLOCK(img_ex)

	sgm::spec::mdo << sgm::spec::empty_line;

	sgm::spec::mdo
	<<	L"The 2nd parameter means image width to show " 
	<<	L"and can be omitted(then it shows the image by original size)."
	<<	sgm::spec::empty_line;
}


static void Guards()
{
	sgm::spec::mdo
	<<	sgm::spec::Title(L"Guards")
	<<	sgm::spec::Title(L"Block Guard", 2)
	<<	sgm::spec::Load_code_block(L"md_guard_ex") << sgm::spec::newl;

BEGIN_CODE_BLOCK(md_guard_ex)
	{
		sgm::spec::md_block_guard mbg;

		sgm::spec::mdo 
		<<	L"Until \"mbg\" is alive(before its destructor is excuted), "
		<<	L"all contents are in box tab." << sgm::spec::newl;
	}
END_CODE_BLOCK(md_guard_ex)


	sgm::spec::mdo
	<<	sgm::spec::newl
	<<	sgm::spec::Title(L"HTML Guard", 2)
	<<	sgm::spec::Load_code_block(L"html_guard_ex") << sgm::spec::newl;

BEGIN_CODE_BLOCK(html_guard_ex)
	{
		sgm::spec::html_block_guard hbg(L"center strong blockquote");
		
		sgm::spec::mdo 
		<<	L"Until \"hbg\" is alive, "
		<<	L"all contents are aligned at center, emphasized and quoted in block." 
		<<	sgm::spec::newl;
	}
END_CODE_BLOCK(html_guard_ex)

	sgm::spec::mdo << sgm::spec::empty_line;
	
	sgm::spec::mdo 
	<<	L"Another way to apply HTML tags is to use \"sgm::spec::HTML_tag\" function."
	<<	sgm::spec::newl
	<<	sgm::spec::Load_code_block(L"html_tag_ex") << sgm::spec::newl;

BEGIN_CODE_BLOCK(html_tag_ex)
	sgm::spec::mdo 
	<<	sgm::spec::HTML_tag(L"HTML tags to a short messages.", L"center strong blockquote");
END_CODE_BLOCK(html_tag_ex)

	
	sgm::spec::mdo << sgm::spec::empty_line;
}


static void Literal_Suffixes()
{
	sgm::spec::mdo
	<<	sgm::spec::Title(L"Literal Suffixes")
	<<	sgm::spec::Title(L"\"\"_mdo for Tabless Description", 2)
	<<	sgm::spec::Load_code_block(L"mdo_suffix_ex") << sgm::spec::newl;

BEGIN_CODE_BLOCK(mdo_suffix_ex)
	sgm::spec::mdo
	<<	LR"(
			If you want to write something verbose, 
			pre-defined literal operator ""_mdo should be helpful.

			As if you write a normal script or comment, 
			describe what you want to explain freely.
					All blank spaces and tabs on left will be removed.
		)"_mdo;
END_CODE_BLOCK(mdo_suffix_ex)

	sgm::spec::mdo << sgm::spec::empty_line;


	sgm::spec::mdo
	<<	sgm::spec::Title(L"\"\"_code for Pseudo Code", 2)
	<<	L"Sometimes, it can be effective to use peudo code which is not excutable "
	<<	L"but easy to understand when you explain a logic and how it works. "
	<<	L"A pre-defined literal operator \"\"_code helps you write the pseudo code very easily."
	<<	sgm::spec::newl
	<<	sgm::spec::Load_code_block(L"pseudo_code_ex") << sgm::spec::newl;

BEGIN_CODE_BLOCK(pseudo_code_ex)
	sgm::spec::mdo 
	<<	LR"(
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
	<<	L"----" << sgm::spec::newl
	<<	sgm::spec::HTML_tag(L"Thank you for watching", L"strong center") << sgm::spec::newl;
}
//========//========//========//========//=======#//========//========//========//========//=======#


SGM_SPECIFICATION_TEST(sgm::spec::Test_, Specification, /**/)
{	::intro
,	::Math_Expression
,	::Code_Block
,	::Assertions
,	::External_Resources
,	::Guards
,	::Literal_Suffixes
,	::outro
};