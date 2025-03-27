import sys
from pysat.card import CardEnc, EncType

def encode_amk(literals, top_id, at_most):
    cnf = CardEnc.atmost(lits=literals, top_id=top_id, bound=at_most, encoding=EncType.cardnetwrk)
    
    all_literals = set()
    for clause in cnf.clauses:
        for lit in clause:
            all_literals.add(abs(lit))

    original_literals = set(literals)
    aux_vars = all_literals - original_literals

    return cnf, top_id + len(aux_vars)

def main():
    if len(sys.argv) != 4:
        print("Usage: python<version> card_ladder_amk.py <number_literals> <width_of_amk_constraint> <at_most_value>")
        sys.exit(1)

    try:
        n = int(sys.argv[1]) # stands for number of literals
        w = int(sys.argv[2]) # stands for width of AMK constraint
        k = int(sys.argv[3]) # stands for at_most value
    except ValueError:
        print("Error: All arguments must be integers.")
        sys.exit(1)

    top_id = n
    formula = []
    
    for i in range (1, n - w + 2):
        literals = list(range(i, i + w))
        add_formula, top_id = encode_amk(literals, top_id, k)
        formula.append(add_formula)

    for amk in formula:
        # print(amk)
        for clause in amk:
            print(" ".join(map(str, clause)) + " 0")

            

if __name__ == "__main__":
    main()