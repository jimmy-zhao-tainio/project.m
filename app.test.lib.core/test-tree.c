#include <stdlib.h>
#include <stdio.h>

#include <lib.core/tree.h>
#include <lib.core/tree-private.h>
#include <lib.core/permute.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <lib.core/defines-private.h>

#include "test-tree.h"

static bool validate_tree                (Tree *tree);
static bool validate_tree_bst            (Tree *tree, TreeNode *node, TreeNode **previous);
static bool validate_tree_aa_rule_1      (Tree *tree, TreeNode *node);
static bool validate_tree_aa_rule_2      (Tree *tree, TreeNode *node);
static bool validate_tree_aa_rule_3      (Tree *tree, TreeNode *node);
static bool validate_tree_aa_rule_4      (Tree *tree, TreeNode *node);
static bool validate_tree_aa_rule_5      (Tree *tree, TreeNode *node);
static bool validate_tree_aa_balance     (Tree *tree);
static bool validate_tree_aa_balance_min (Tree *tree, TreeNode *node);
static bool validate_tree_aa_balance_max (Tree *tree, TreeNode *node);
static bool validate_tree_iterator       (Tree *tree);

static size_t shuffled[1000] = { 
	239, 330, 172, 665, 826, 770, 123, 567, 289, 168, 965, 120, 393, 762, 210, 523, 713, 767, 387,   0,
	425, 260,  98,  24, 955, 385, 778, 415, 170, 789, 658, 872, 329, 227, 206, 480, 135, 231, 706, 328,
	208, 214, 475, 846, 575, 852, 574, 148, 209, 456,  55, 380, 278, 849, 959, 976, 718, 908, 865, 436,
	 14, 350, 835, 438, 410, 594, 630, 730, 763, 855, 941, 499,  66, 279, 986, 352, 670, 824, 688, 761,
	804, 610, 348, 383, 559, 314, 437, 996, 431, 716, 988, 273, 549, 547, 747, 715, 685, 454, 581, 702,
	122, 341, 841, 857, 423, 416, 915, 321, 115, 775, 474, 796, 207,  64, 940, 660, 952, 933,  32, 131,
	655, 246, 833, 219, 663,  79,   5, 270, 100,  39, 674, 196, 800, 646, 743, 408, 813, 592, 218, 546,
	569, 900, 712, 357, 558, 913, 905, 675, 347,  12, 516, 944,  68, 258,  61, 920, 363, 810,  44, 165,
	768, 587, 783, 590, 582, 577, 854, 240, 960, 521, 698, 885, 435, 156,  77, 903, 585, 466, 244, 721,
	497, 327, 928, 382, 179, 315, 460, 831, 651, 923, 629, 378, 285, 985, 626, 634, 489, 345, 251, 354,
	333, 163, 593, 708, 112,  73, 591, 622, 687, 140, 948, 758, 924, 531, 249, 656, 223, 306, 619, 597,
	381, 464, 811, 366, 701, 942, 457, 560,  16, 395, 935,  30, 463, 932, 388, 162, 178, 344,  60, 133,
	242, 616, 125, 411,  86,  82,  84, 744, 648, 803, 929, 441, 261, 881, 589, 368, 277, 307, 264, 312,
	477, 295, 390, 362, 723, 703, 169, 625, 973, 890, 642, 391, 700, 451, 613, 607, 863, 282, 707, 482,
	856, 522, 467, 756, 492, 502, 686, 954, 722, 791, 150,  70, 859, 792, 967, 780, 878, 977, 842, 364,
	742, 114, 375, 764, 313, 653, 745,   6,  33,  41, 498, 969,  15, 844, 194, 896, 617, 291, 147, 689,
	370, 571,  88, 117, 103, 530,  72, 735, 351, 403, 573, 899, 962, 339, 724,  26, 596, 532,   1, 317,
	552,  31, 772, 232, 263, 453, 956, 119, 190, 501, 243, 309,  57, 505, 406, 880, 957, 636, 684, 993,
	228, 864, 439, 536, 897, 717, 926, 823, 951, 981, 975, 679,  59, 561,  89, 672, 793, 359, 873, 671,
	 67, 127, 728, 554, 447, 729, 237, 851, 705, 906,  22, 346,   4, 676, 326,  35, 979, 788, 241,  23,
	904, 576, 151, 925, 529, 840, 681,  56, 128, 875, 323, 898, 420, 876, 765, 392, 556, 916,  58, 850,
	727,  21, 669, 838, 484, 400, 564, 310, 372, 430, 752, 259, 478, 308, 550, 142, 539,  80, 290, 269,
	275, 274, 361, 711, 296, 917, 678, 766, 412, 138, 106, 213, 488, 909, 146,  85,  87, 283, 939, 191,
	998, 862, 113, 615, 568,  65, 293, 771, 919, 860, 417, 513, 937, 867, 599, 154, 545, 199, 553, 635,
	578, 807, 180, 825, 548, 911, 177, 714, 184, 520, 377, 972, 869,  11, 126, 235, 236,  71, 583,  83,
	193, 349, 384, 212, 396, 773, 736, 827, 485, 946, 369, 808, 173, 608, 720, 883, 418, 121, 861, 641,
	921, 894, 336, 737, 779,  25, 628,  97, 828, 342, 284, 211, 528, 503, 455, 627, 749, 507, 936, 922,
	879, 443, 866, 245, 292,  74, 785,  62, 794, 157, 757, 397, 750, 982, 910, 252,  46, 340, 586, 221,
	153, 182, 740,  20, 257, 918, 884, 304, 421, 598, 355, 494, 183, 444, 525, 621, 677, 428, 105,  91,
	650, 508, 201, 984, 445, 271, 611, 839, 533, 805, 376,  28, 247, 543, 318, 373, 692, 136, 971, 205,
	220,  13, 874, 580, 481, 907, 691, 618,   7, 798, 167,  10, 281, 930,  47, 699, 624, 166, 820, 360,
	433, 978, 442, 777, 891, 623, 461, 734,  38, 301, 974, 664, 739, 683, 832, 647, 452, 963, 853, 335,
	774, 471, 129, 427, 405, 645, 719, 991, 459, 109, 895, 695, 253, 222, 751, 164, 404, 776, 189, 819,
	540, 506, 486, 837, 512, 472, 158, 659, 518, 945, 300,  69, 267, 298, 821, 888, 414, 882, 511, 280,
	643, 992, 510, 365, 143, 797, 250, 322, 248, 465, 858, 517, 238, 462, 469, 950, 171, 338,  52,  75,
	709, 961, 557, 755, 602, 118, 958, 584, 600, 229, 198, 637, 200,  29, 997, 845, 787, 426, 192,  37,
	816, 399, 604, 401,  63, 579, 422, 606, 886, 801, 155, 493, 448,  50,  81, 500, 603, 990, 802, 970,
	449, 588, 132, 104, 254, 216, 914, 159, 409, 847, 320, 633,  36, 139, 652, 893,  49, 432, 302, 356,
	234, 538, 644, 116, 746, 565, 473, 812, 299, 912, 782, 174, 639, 286, 224, 255, 187, 515, 541, 262,
	748, 319, 983, 424,   3, 440, 697,   2, 889, 901, 175, 134, 204, 101, 230, 534, 496, 337, 725, 817,
	188, 682,  76, 450, 429, 111, 609, 266, 696, 943, 806,   9, 446,  17, 394, 458,  78, 809,  53, 640,
	 27, 124, 814, 570, 324, 822, 673, 504, 731, 994, 887, 836, 353, 311,   8, 966, 753, 544, 605, 542,
	563, 631, 110, 795, 379, 524, 759, 949, 790, 145, 694,  95, 367, 107, 185, 419, 551, 226, 144, 371,
	693, 343, 476, 272, 161, 732,  92, 389, 265, 662, 784, 799, 276, 938, 203, 141, 649, 834,  48, 386,
	374,  96, 160, 595, 868,  93, 612, 964, 303, 176, 741,  54,  51, 968, 294, 197,  43, 316, 509, 535,
	786,  34, 526, 769, 999, 668, 108, 987,  90, 995, 726,  99, 555, 527, 620, 738, 215, 638, 332, 288,
	268, 934, 256, 483,  40, 519, 490, 666, 491, 848, 537,  42, 661, 947, 572, 781, 601, 614, 470, 487,
	931, 102, 733, 181, 202, 152, 902, 989, 149, 305, 815,  45, 186, 657, 130, 514, 398, 953, 331, 566,
	680, 334, 654, 754,  19, 927, 297, 137, 195, 704, 710, 407, 225, 562, 217, 892, 760, 818, 667, 870,
	402, 325, 830,  94, 434, 829, 843, 358, 287, 468, 877, 495, 233, 871, 413,  18, 980, 632, 690, 479 };

bool test_tree_create_function_call_1 (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (Tree) - 1);
	CATCH (tree_create ());
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).line != 22);
	PASS ();
}

bool test_tree_create_function_call_2 (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (Tree) + sizeof (TreeNode) - 1);
	CATCH (tree_create ());
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).line != 27);
	PASS ();
}

bool test_tree_create (Test *test)
{
	Tree *tree;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	tree_destroy (tree);
	PASS ();
}

bool test_tree_node_insert_function_call_1 (Test *test)
{
	Tree *tree;
	Object unique;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	memory_commit_limit (memory_commit_size ());
	CATCH (tree_node_insert (tree, tree->root, &unique, "b"));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_node_insert_function_call_2 (Test *test)
{
	Tree *tree;
	Object a = { 0 }, b= { 1 };

	TITLE ();
	CATCH (!(tree = tree_create ()));
	CATCH (!tree_insert (tree, &a, "a"));
	memory_commit_limit (memory_commit_size ());
	CATCH (tree_node_insert (tree, tree->root, &b, "b"));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_node_insert_invalid_operation (Test *test)
{
	Tree *tree;
	Object a = { 0 };

	TITLE ();
	CATCH (!(tree = tree_create ()));
	CATCH (!tree_insert (tree, &a, "a"));
	CATCH (tree_insert (tree, &a, "a"));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_insert_invalid_argument_1 (Test *test)
{
	Object a = { 0 };

	TITLE ();
	CATCH (tree_insert (NULL, &a, "b"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_tree_insert_invalid_argument_2 (Test *test)
{
	Tree *tree;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	CATCH (tree_insert (tree, NULL, "b"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_insert_overflow (Test *test)
{
	Tree *tree;
	Object a = { 0 };

	TITLE ();
	CATCH (!(tree = tree_create ()));
	size_t_private_max (0);
	CATCH (tree_insert (tree, &a, "b"));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorOverflow);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_insert_function_call_1 (Test *test)
{
	Tree *tree;
	Object unique;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	memory_commit_limit (memory_commit_size ());
	CATCH (tree_insert (tree, &unique, "b"));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_insert_function_call_2 (Test *test)
{
	Tree *tree;
	Object a = { 0 }, b= { 1 };

	TITLE ();
	CATCH (!(tree = tree_create ()));
	CATCH (!tree_insert (tree, &a, "a"));
	memory_commit_limit (memory_commit_size ());
	CATCH (tree_insert (tree, &b, "b"));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	tree_destroy (tree);
	PASS ();
}
bool test_tree_insert_invalid_operation (Test *test)
{
	Tree *tree;
	Object a = { 0 };

	TITLE ();
	CATCH (!(tree = tree_create ()));
	CATCH (!tree_insert (tree, &a, "a"));
	CATCH (tree_insert (tree, &a, "a"));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_insert_1 (Test *test)
{
	Tree *tree;
	Object array[10] = { { 0 }, { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 } };
	size_t i;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	for (i = 0; i < 10; i++) {
		CATCH (!tree_insert (tree, &array[i], NULL));
		CATCH (!validate_tree (tree));
		CATCH (tree_count (tree) != i + 1);
	}
	tree_destroy (tree);
	PASS ();
}

bool test_tree_insert_2 (Test *test)
{
	Tree *tree;
	Object array[10] = { { 0 }, { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 } };
	size_t i;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	for (i = 10; i-- > 0;) {
		CATCH (!tree_insert (tree, &array[i], NULL));
		CATCH (!validate_tree (tree));
		CATCH (tree_count (tree) != 10 - i);
	}
	tree_destroy (tree);
	PASS ();
}

bool test_tree_insert_3 (Test *test)
{
	Permute *permute;
	const size_t *next;
	Tree *tree;
	Object array_7[7];
	size_t i;

	TITLE ();
	CATCH (!(permute = permute_create (7)));
	while ((next = permute_next (permute))) {
		CATCH (!(tree = tree_create ()));
		for (i = 0; i < 7; i++) {
			array_7[i].id = (unsigned long long)next[i];
			CATCH (!tree_insert (tree, &array_7[i], NULL));
			CATCH (!validate_tree (tree));
			CATCH (tree_count (tree) != i + 1);
		}
		tree_destroy (tree);
	}
	permute_destroy (permute);
	PASS ();
}

bool test_tree_insert_4 (Test *test)
{
	Permute *permute;
	const size_t *next;
	Tree *tree;
	Object array_10[10];
	Object array_7[7];
	size_t i;

	TITLE ();
	CATCH (!(permute = permute_create (7)));
	while ((next = permute_next (permute))) {
		CATCH (!(tree = tree_create ()));
		for (i = 0; i < 10; i++) {
			array_10[i].id = (unsigned long long)(i + 1);
			CATCH (!tree_insert (tree, &array_10[i], NULL));
			CATCH (!validate_tree (tree));
			CATCH (tree_count (tree) != i + 1);
		}
		for (i = 0; i < 7; i++) {
			array_7[i].id = (unsigned long long)(10 + next[i]);
			CATCH (!tree_insert (tree, &array_7[i], NULL));
			CATCH (!validate_tree (tree));
			CATCH (tree_count (tree) != 10 + i + 1);
		}
		tree_destroy (tree);
	}
	permute_destroy (permute);
	PASS ();
}

bool test_tree_insert_5 (Test *test)
{
	Permute *permute;
	const size_t *next;
	Tree *tree;
	Object array_10[10];
	Object array_7[7];
	size_t i;

	TITLE ();
	CATCH (!(permute = permute_create (7)));
	while ((next = permute_next (permute))) {
		CATCH (!(tree = tree_create ()));
		i = 10;
		while (i-- > 0) {
			array_10[i].id = (unsigned long long)i;
			CATCH (!tree_insert (tree, &array_10[i], NULL));
			CATCH (!validate_tree (tree));
			CATCH (tree_count (tree) != 10 - i);
		}
		for (i = 0; i < 7; i++) {
			array_7[i].id = (unsigned long long)(10 + next[i]);
			CATCH (!tree_insert (tree, &array_7[i], NULL));
			CATCH (!validate_tree (tree));
			CATCH (tree_count (tree) != 10 + i + 1);
		}
		tree_destroy (tree);
	}
	permute_destroy (permute);
	PASS ();
}

bool test_tree_insert_6 (Test *test)
{
	Tree *tree;
	Object array[1000];
	size_t i;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	for (i = 0; i < 1000; i++) {
		array[i].id = (unsigned long long)shuffled[i];
		CATCH (!tree_insert (tree, &array[i], NULL));
		CATCH (!validate_tree (tree));
		CATCH (tree_count (tree) != i + 1);
	}
	tree_destroy (tree);
	PASS ();
}

bool test_tree_search_invalid_argument_1 (Test *test)
{
	Tree *tree;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	CATCH (tree_search (tree, NULL));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_search_invalid_argument_2 (Test *test)
{
	Object key = { 1 };

	TITLE ();
	CATCH (tree_search (NULL, &key));
	PASS ();
}

bool test_tree_search_1 (Test *test)
{
	Tree *tree;
	Object key = { 1 };

	TITLE ();
	CATCH (!(tree = tree_create ()));
	CATCH (!tree_insert (tree, &key, &key));
	CATCH (tree_search (tree, &key) != &key);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_search_2 (Test *test)
{
	Tree *tree;
	Object keys[2] = { { 1 }, { 2 } };

	TITLE ();
	CATCH (!(tree = tree_create ()));
	CATCH (!tree_insert (tree, &keys[0], &keys[0]));
	CATCH (!tree_insert (tree, &keys[1], &keys[1]));
	CATCH (tree_search (tree, &keys[0]) != &keys[0]);
	CATCH (tree_search (tree, &keys[1]) != &keys[1]);
	tree_destroy (tree);
	CATCH (!(tree = tree_create ()));
	CATCH (!tree_insert (tree, &keys[0], &keys[0]));
	CATCH (!tree_insert (tree, &keys[1], &keys[1]));
	CATCH (tree_search (tree, &keys[1]) != &keys[1]);
	CATCH (tree_search (tree, &keys[0]) != &keys[0]);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_search_3 (Test *test)
{
	Tree *tree;
	Object keys[10] = { { 0 }, { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 } };
	size_t i;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	for (i = 0; i < 10; i++) {
		CATCH (!tree_insert (tree, &keys[i], &keys[i]));
	}
	for (i = 0; i < 10; i++) {
		CATCH (tree_search (tree, &keys[i]) != &keys[i]);
	}
	tree_destroy (tree);
	PASS ();
}

bool test_tree_search_4 (Test *test)
{
	Permute *permute;
	const size_t *next;
	Tree *tree;
	Object array_7[7];
	size_t i;

	TITLE ();
	CATCH (!(permute = permute_create (7)));
	while ((next = permute_next (permute))) {
		CATCH (!(tree = tree_create ()));
		for (i = 0; i < 7; i++) {
			array_7[i].id = next[i];
			tree_insert (tree, &array_7[i], &array_7[i]);
		}
		for (i = 0; i < 7; i++) {
			CATCH (tree_search (tree, &array_7[i]) != &array_7[i]);
		}
		tree_destroy (tree);
	}
	permute_destroy (permute);
	PASS ();
}

bool test_tree_search_5 (Test *test)
{
	Permute *permute;
	const size_t *next;
	Tree *tree;
	Object array_10[10];
	Object array_7[7];
	size_t i;

	TITLE ();
	CATCH (!(permute = permute_create (7)));
	while ((next = permute_next (permute))) {
		CATCH (!(tree = tree_create ()));
		for (i = 0; i < 10; i++) {
			array_10[i].id = i + 1;
			CATCH (!tree_insert (tree, &array_10[i], &array_10[i]));
		}
		for (i = 0; i < 7; i++) {
			array_7[i].id = 10 + next[i];
			CATCH (!tree_insert (tree, &array_7[i], &array_7[i]));
		}
		for (i = 0; i < 10; i++) {
			CATCH (tree_search (tree, &array_10[i]) != &array_10[i]);
		}
		for (i = 0; i < 7; i++) {
			CATCH (tree_search (tree, &array_7[i]) != &array_7[i]);
		}
		tree_destroy (tree);
	}
	permute_destroy (permute);
	PASS ();
}

bool test_tree_search_6 (Test *test)
{
	Permute *permute;
	const size_t *next;
	Tree *tree;
	Object array_10[10];
	Object array_7[7];
	size_t i;

	TITLE ();
	CATCH (!(permute = permute_create (7)));
	while ((next = permute_next (permute))) {
		CATCH (!(tree = tree_create ()));
		i = 10;
		while (i-- > 0) {
			array_10[i].id = i;
			CATCH (!tree_insert (tree, &array_10[i], &array_10[i]));
		}
		for (i = 0; i < 7; i++) {
			array_7[i].id = 10 + next[i];
			CATCH (!tree_insert (tree, &array_7[i], &array_7[i]));
		}
		i = 10;
		while (i-- > 0) {
			CATCH (tree_search (tree, &array_10[i]) != &array_10[i]);
		}
		for (i = 0; i < 7; i++) {
			CATCH (tree_search (tree, &array_7[i]) != &array_7[i]);
		}
		tree_destroy (tree);
	}
	permute_destroy (permute);
	PASS ();
}

bool test_tree_search_7 (Test *test)
{
	Tree *tree;
	Object array[1000];
	size_t i;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	for (i = 0; i < 1000; i++) {
		array[i].id = shuffled[i];
		CATCH (!tree_insert (tree, &array[i], &array[i]));
	}
	for (i = 0; i < 1000; i++) {
		CATCH (((Object *)tree_search (tree, &array[i]))->id != array[i].id);
	}
	tree_destroy (tree);
	PASS ();
}

bool test_tree_iterator_path_size_1 (Test *test)
{
	Tree *tree;
	TreeIterator *iterator;

	TITLE ();
	tree_iterator_path_size (0);
	CATCH (!(tree = tree_create ()));
	CATCH (!(iterator = tree_iterator_create (tree)));
	CATCH (memory_commit_size () !=
	      sizeof (size_t) + sizeof (Tree) +
	      sizeof (size_t) + sizeof (TreeNode) +
	      sizeof (size_t) + sizeof (TreeIterator) + 
	      sizeof (size_t) + ((sizeof (TreeNode *) * TREE_ITERATOR_PATH_SIZE)));
	tree_iterator_destroy (iterator);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_iterator_path_size_2 (Test *test)
{
	Tree *tree;
	TreeIterator *iterator;
	unsigned long long size;

	TITLE ();
	tree_iterator_path_size (123);
	CATCH (!(tree = tree_create ()));
	size = memory_commit_size ();
	CATCH (!(iterator = tree_iterator_create (tree)));
	CATCH (memory_commit_size () !=
	      sizeof (size_t) + sizeof (Tree) +
	      sizeof (size_t) + sizeof (TreeNode) +
	      sizeof (size_t) + sizeof (TreeIterator) + 
	      sizeof (size_t) + ((sizeof (TreeNode *) * 123)));
	tree_iterator_destroy (iterator);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_iterator_create_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (!tree_iterator_create (NULL));
	CATCH (error_count () != 0);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_tree_iterator_create_function_call_1 (Test *test)
{
	Tree *tree;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	memory_commit_limit (memory_commit_size ());
	CATCH (tree_iterator_create (tree));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).line != 139);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_iterator_create_function_call_2 (Test *test)
{
	Tree *tree;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	memory_commit_limit (memory_commit_size () + sizeof (TreeIterator) + sizeof (size_t));
	CATCH (tree_iterator_create (tree));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).line != 144);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_iterator_create (Test *test)
{
	Tree *tree;
	TreeIterator *iterator;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	CATCH (!(iterator = tree_iterator_create (tree)));
	tree_iterator_destroy (iterator);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_iterator_next_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (tree_iterator_next (NULL));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_tree_iterator_next_overflow (Test *test)
{
	Tree *tree;
	TreeIterator *iterator;
	Object keys[3] = { { 0 }, { 1 }, { 2 } };
	size_t i;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	for (i = 0; i < 3; i++) {
		CATCH (!(tree_insert (tree, &keys[i], &keys[i])));
	}
	tree_iterator_path_size (1);
	CATCH (!(iterator = tree_iterator_create (tree)));
	CATCH (tree_iterator_next (iterator));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorOverflow);
	tree_iterator_destroy (iterator);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_iterator_next_1 (Test *test)
{
	Tree *tree;
	TreeIterator *iterator;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	iterator = tree_iterator_create (tree);
	CATCH (tree_iterator_next (iterator));
	CATCH (iterator->key || iterator->value);
	CATCH (tree_iterator_next (iterator));
	CATCH (iterator->key || iterator->value);
	tree_iterator_destroy (iterator);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_iterator_next_2 (Test *test)
{
	Tree *tree;
	TreeIterator *iterator;
	Object key = { 1 };
	char *value = "value";

	TITLE ();
	CATCH (!(tree = tree_create ()));
	CATCH (!(iterator = tree_iterator_create (tree)));
	CATCH (!tree_insert (tree, &key, value));
	CATCH (!tree_iterator_next (iterator));
	CATCH (iterator->key != &key || iterator->key->id != key.id || iterator->value != value);
	CATCH (tree_iterator_next (iterator));
	CATCH (iterator->key || iterator->value);
	tree_iterator_destroy (iterator);
	CATCH (!(iterator = tree_iterator_create (tree)));
	CATCH (!tree_iterator_next (iterator));
	CATCH (iterator->key != &key || iterator->key->id != key.id || iterator->value != value);
	CATCH (tree_iterator_next (iterator));
	CATCH (iterator->key || iterator->value);
	tree_iterator_destroy (iterator);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_iterator_next_3 (Test *test)
{
	Tree *tree;
	TreeIterator *iterator;
	Object array[1000];
	size_t i;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	for (i = 0; i < 1000; i++) {
		array[i].id = shuffled[i];
		CATCH (!tree_insert (tree, &array[i], &array[i]));
	}
	CATCH (!(iterator = tree_iterator_create (tree)));
	for (i = 0; i < 1000; i++) {
		CATCH (!tree_iterator_next (iterator));
		CATCH (iterator->key->id != i);
	}
	tree_iterator_destroy (iterator);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_iterator_destroy_invalid_argument (Test *test)
{
	TITLE ();
	tree_iterator_destroy (NULL);
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_tree_iterator_destroy (Test *test)
{
	Tree *tree;
	TreeIterator *iterator;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	CATCH (!(iterator = tree_iterator_create (tree)));
	tree_iterator_destroy (iterator);
	tree_destroy (tree);
	PASS ();
}

bool test_tree_count_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (tree_count (NULL) != 0);
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_tree_count (Test *test)
{
	Tree *tree;
	Object array[10];
	size_t i;

	TITLE ();
	CATCH (!(tree = tree_create ()));
	CATCH (tree_count (tree) != 0);
	for (i = 0; i < 10; i++) {
		array[i].id = i;
		CATCH (!tree_insert (tree, &array[i], &array[i]));
		CATCH (tree_count (tree) != i + 1);
	}
	tree_destroy (tree);
	PASS ();
}

static bool validate_tree (Tree *tree)
{
	TreeNode *previous = NULL;

	if (!validate_tree_bst (tree, tree->root, &previous) ||
	    !validate_tree_aa_rule_1 (tree, tree->root) ||
	    !validate_tree_aa_rule_2 (tree, tree->root) ||
	    !validate_tree_aa_rule_3 (tree, tree->root) ||
	    !validate_tree_aa_rule_4 (tree, tree->root) ||
	    !validate_tree_aa_rule_5 (tree, tree->root) ||
	    !validate_tree_aa_balance (tree) ||
	    !validate_tree_iterator (tree)) {
		return false;
	}
	return true;
}

static bool validate_tree_bst (Tree *tree, TreeNode *node, TreeNode **previous)
{
	if (node == tree->nil) {
		return true;
	}
	if (!validate_tree_bst (tree, node->link[0], previous)) {
		return false;
	}
	if (*previous != NULL && node->key->id < (*previous)->key->id) {
		return false;
	}
	*previous = node;
	if (!validate_tree_bst (tree, node->link[1], previous)) {
		return false;
	}
	return true;
}

static bool validate_tree_aa_rule_1 (Tree *tree, TreeNode *node)
{
	if (node == tree->nil) {
		return true;
	}
	/* Leaf nodes are at level one. */
	if (node->link[0] == tree->nil &&
		node->link[1] == tree->nil) {
		if (node->level == 1) {
			return true;
		}
		else {
			return false;
		}
	}
	if (node->link[0] != tree->nil &&
		node->link[1] != tree->nil) {
		if (!validate_tree_aa_rule_1 (tree, node->link[0])) {
			return false;
		}
		return validate_tree_aa_rule_1 (tree, node->link[1]);
	}
	if (node->link[0] != tree->nil) {
		return validate_tree_aa_rule_1 (tree, node->link[0]);
	}
	else {
		return validate_tree_aa_rule_1 (tree, node->link[1]);
	}
}

static bool validate_tree_aa_rule_2 (Tree *tree, TreeNode *node)
{
	/* Left children must have a level less than their parent. */
	if (node == tree->nil) {
		return true;
	}
	if (node->level != node->link[0]->level + 1) {
		return false;
	}
	if (!validate_tree_aa_rule_2 (tree, node->link[0])) {
		return false;
	}
	return validate_tree_aa_rule_2 (tree, node->link[1]);
}

static bool validate_tree_aa_rule_3 (Tree *tree, TreeNode *node)
{
	/* Right children must have a level less than or equal to their parent. */
	if (node == tree->nil) {
		return true;
	}
	if (node->level != node->link[1]->level &&
	    node->level != node->link[1]->level + 1) {
		return false;
	}
	if (!validate_tree_aa_rule_3 (tree, node->link[0])) {
		return false;
	}
	return validate_tree_aa_rule_3 (tree, node->link[1]);
}

static bool validate_tree_aa_rule_4 (Tree *tree, TreeNode *node)
{
	/* Right grandchildren must have a level less than their grandparent. */
	if (node == tree->nil) {
		return true;
	}
	if (node->level == node->link[1]->level) {
		/* GP:X P:X GC:X-1 */
		if (node->link[1]->level != node->link[1]->link[1]->level + 1) {
			return false;
		}
	}
	else if (node->level == node->link[1]->level + 1) {
		/* GP:X P:X-1 GC:X-1 */
		/* GP:X P:X-1 GC:X-2 */
		if (node->link[1]->level != node->link[1]->link[1]->level &&
		    node->link[1]->level != node->link[1]->link[1]->level + 1) {
			return false;
		}
	}
	else {
		/* GP:? P:? GC:? */
		return false;
	}
	if (!validate_tree_aa_rule_4 (tree, node->link[0])) {
		return false;
	}
	return validate_tree_aa_rule_4 (tree, node->link[1]);
}

static bool validate_tree_aa_rule_5 (Tree *tree, TreeNode *node)
{
	/* Any node with a level higher than one has to have two children. */
	if (node == tree->nil) {
		return true;
	}
	if (node->level > 1) {
		if (node->link[0] == tree->nil &&
		    node->link[1] == tree->nil) {
			return false;
		}
	}
	if (!validate_tree_aa_rule_5 (tree, node->link[0])) {
		return false;
	}
	return validate_tree_aa_rule_5 (tree, node->link[1]);
}

static bool validate_tree_aa_balance (Tree *tree)
{
	size_t min;
	size_t max;

	min = validate_tree_aa_balance_min (tree, tree->root);
	max = validate_tree_aa_balance_max (tree, tree->root);
	if (min > max) {
		return false;
	}
	if (max - min <= 1) {
		return true;
	}
	return false;
}

static bool validate_tree_aa_balance_max (Tree *tree, TreeNode *node)
{
	size_t left;
	size_t right;
	size_t right_pseudo = 0;

	if (node == tree->nil) {
		return false;
	}
	left = validate_tree_aa_balance_max (tree, node->link[0]);
	right = validate_tree_aa_balance_max (tree, node->link[1]);
	if (node->link[1]->level == node->level) {
		right_pseudo = 1;
	}
	if (left > right) {
		return 1 + left;
	}
	return (right_pseudo ? 0 : 1) + right;
}

static bool validate_tree_aa_balance_min (Tree *tree, TreeNode *node)
{
	size_t left;
	size_t right;
	size_t right_pseudo = 0;

	if (node == tree->nil) {
		return false;
	}
	left = validate_tree_aa_balance_min (tree, node->link[0]);
	right = validate_tree_aa_balance_min (tree, node->link[1]);
	if (node->link[1]->level == node->level) {
		right_pseudo = 1;
	}
	if (left < right) {
		return 1 + left;
	}
	return (right_pseudo ? 0 : 1) + right;
}

static bool validate_tree_iterator (Tree *tree)
{
	TreeIterator *iterator = tree_iterator_create (tree);
	size_t count = 0;
	Object *previous = NULL;
	
	while (tree_iterator_next (iterator)) {
		if (count == SIZE_MAX) {
			return false;
		}
		count++;
		if (!previous) {
			previous = iterator->key;
			continue;
		}
		if (previous->id >= iterator->key->id) {
			tree_iterator_destroy (iterator);
			return false;
		}
		previous = iterator->key;
	}
	tree_iterator_destroy (iterator);
	if (count != tree->count) {
		return false;
	}
	return true;
}
