import sys
from pypblib import pblib

def encode_amk(w, first_literals, first_free_var, at_most):
    # print("# Encode w = " + str(w) + ", first = " + str(first_literals) + ", first_free = " + str(first_free_var) + ", at_most = " + str(at_most) + ".")
    pb2 = pblib.Pb2cnf()
    config = pblib.PBConfig()
    config.set_AMK_Encoder(pblib.AMK_BDD)
    
    literals = list(range(first_literals, first_literals + w))
    formula = []
    max_var = pb2.encode_at_most_k(literals, at_most, formula, first_free_var)
    
    return formula, max_var

def main():
    if len(sys.argv) != 4:
        print("Usage: python<version> bdd_ladder_amk.py <number_literals> <width_of_amk_constraint> <at_most_value>")
        sys.exit(1)

    try:
        n = int(sys.argv[1]) # stands for number of literals
        w = int(sys.argv[2]) # stands for width of AMK constraint
        k = int(sys.argv[3]) # stands for at_most value
    except ValueError:
        print("Error: All arguments must be integers.")
        sys.exit(1)

    first_free_var = n + 1
    formula = []
    
    for i in range (1, n - w + 2):
        add_formula, new_max_var = encode_amk(w, i, first_free_var, k)
        formula.append(add_formula)
        first_free_var = new_max_var + 1

    for amk in formula:
        # print(amk)
        for clause in amk:
            print(" ".join(map(str, clause)) + " 0")

            

if __name__ == "__main__":
    main()