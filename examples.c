#include "sumtype.h"
#include <stdio.h>
#include <stdint.h>

struct Node {
	struct BinaryTree *l;
	int x;
	struct BinaryTree *r;
};

Sumtype(
	BinaryTree,
	(int, leaf),
	(struct Node, node)
)

int sum(struct BinaryTree *tree) {
#ifdef Sumtype_typeinference
	match_t(tree) {
		let_t(leaf) return *leaf;
		let_t(node) return sum(node->l) + node->x + sum(node->r);
	};
#else
	match(struct BinaryTree, tree) {
		let(int, leaf) return *leaf;
		let(struct Node, node) return sum(node->l) + node->x + sum(node->r);
	}
#endif
	// Unreachable
	return 0;
}

Sumtype(Haha, (char *, name), (uint16_t, id), (uint32_t, var))

Sumtype(
	ManyVariants,
	(char *, var0),
	(char *, var1),
	(char *, var2),
	(char *, var3),
	(char *, var4),
	(char *, var5),
	(char *, var6),
	(char *, var7),
	(char *, var8),
	(char *, var9),
	(char *, var10),
	(char *, var11),
	(char *, var12),
	(char *, var13),
	(char *, var14),
	(char *, var15),
	(char *, var16),
	(char *, var17),
	(char *, var18),
	(char *, var19),
	(char *, var20),
	(char *, var21),
	(char *, var22),
	(char *, var23),
	(char *, var24),
	(char *, var25),
	(char *, var26),
	(char *, var27),
	(char *, var28),
	(char *, var29),
	(char *, var30),
	(char *, var31),
)

int
main() {
	struct Haha something = Haha_name("hello");

#ifdef Sumtype_typeinference
	match_t(&something) {
		let_t(name) {
	    		printf("%s\n", *name);
		}
		let_t(id) {
	    		printf("%d\n", *id);
		}
		otherwise {
	    		printf("var\n");
		}
	};
#else
	match(struct Haha, &something) {
		let(char *, name) {
	    		printf("%s\n", *name);
		}
		let(uint16_t, id) {
	    		printf("%d\n", *id);
		}
		otherwise {
	    		printf("var\n");
		}
	};
#endif

	if (MATCHES(name, &something)) puts("matches!");

	struct ManyVariants many = ManyVariants_var31("damn");

#ifdef Sumtype_typeinference
	iflet_t(var31, &many) {
		printf("%s\n", *var31);
	}
#else
	iflet(struct ManyVariants, char *, var31, &many) {
		printf("%s\n", *var31);
	}
#endif
}
