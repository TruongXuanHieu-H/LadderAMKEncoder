import sys
from pypblib import pblib

def encode_amo(n, first_literals, first_free_var):
    # print("# Encode n = " + str(n) + ", first = " + str(first_literals) + ", first_free = " + str(first_free_var))
    pb2 = pblib.Pb2cnf()
    config = pblib.PBConfig()
    config.set_AMK_Encoder(pblib.AMK_CARD)
    
    literals = list(range(first_literals, first_literals + n))
    formula = []
    max_var = pb2.encode_at_most_k(literals, 1, formula, first_free_var)
    aux_vars = list(range(first_free_var, max_var + 1))
    
    return formula, aux_vars

def main():
    if len(sys.argv) != 4:
        print("Usage: python<version> card_amo.py <number_literals> <first_literals> <first_free_var>")
        sys.exit(1)

    try:
        n = int(sys.argv[1])
        first_literals = int(sys.argv[2])
        first_free_var = int(sys.argv[3])
    except ValueError:
        print("Error: All arguments must be integers.")
        sys.exit(1)

    formula, aux_vars = encode_amo(n, first_literals, first_free_var)
    
    print(" ".join(map(str, aux_vars)))
    for clause in formula:
        print(" ".join(map(str, clause)))

            

if __name__ == "__main__":
    main()