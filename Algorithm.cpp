//
// Created by Umato on 30.10.2023.
//

#include "Algorithm.h"

#pragma region Stack_Functions

stack* create_stack() {
    stack *s = (stack*) malloc(sizeof(stack));
    if (!s) return nullptr;
    s->top = nullptr;
    return s;
}

void push(stack *s, int element) {
    node *new_node = (node*) malloc(sizeof(node));
    if (!new_node) return;

    new_node->val = element;
    new_node->next = s->top;
    s->top = new_node;
}

int pop(stack *s) {
    if (is_stack_empty(s)) return -1;

    node *temp = s->top;
    int popped_val = temp->val;
    s->top = s->top->next;
    free(temp);
    return popped_val;
}

bool is_stack_empty(stack *s) {
    return s->top == nullptr;
}

int stack_top(stack *s) {
    if (s->top != nullptr) return s->top->val;
    return -1;
}

void free_stack(stack *s) {
    if (!s) return;
    while (!is_stack_empty(s)) {
        pop(s);
    }
    free(s);
}

#pragma endregion

#pragma region Algorithms

ll euclid(ll a, ll b) {
    ll temp;
    while (b != 0) {
        temp = b;
        b = a % b;
        a = temp;
    }
    return abs(a);
}

ll euclid_recursive(const ll a, const ll b) {
    return a ? euclid_recursive(abs(b % a), abs(a)) : b;
}

ll euclid_binary(ll x, ll y) {
    if (x == 0) return y;
    if (y == 0) return x;

    ll power = 0;
    while (((x | y) & 1) == 0) {
        ++power;
        x >>= 1;
        y >>= 1;
    }
    while (x) {
        while ((y & 1) == 0) y >>= 1;
        while ((x & 1) == 0) x >>= 1;

        if (y > x) swap(x, y);

        x -= y;
    }
    return y << power;
}

ll mod_pow(ll x, ll y, ll m) {
    if (y == 0) return 1;

    ll ans = 1;
    x %= m;
    while (y) {
        if (y & 1) ans = (ans * x) % m;
        x = (x * x) % m;
        y >>= 1;
    }
    return ans;
}

ll mod_pow_recursive(const ll x, const ll y, const ll m) {
    if (y == 0) return 1;

    ll h = mod_pow_recursive(x, y >> 1, m);
    h = (h * h) % m;
    if ((y & 1) == 0) return h;
    else return (x % m * h) % m;
}

ll lr_mod_pow(const ll x, const ll y, const ll m) {
    if (y == 0) return 1;

    ll pow = 1;
    while (pow < y) pow <<= 1;

    ll result = x % m;
    for (pow >>= 2; pow; pow >>= 1) {
        result = (result * result) % m;
        if (pow & y) result = (result * x) % m;
    }
    return result;
}

ll euclid_extended_recursive(const ll a, const ll b, ll &x, ll &y) {
    if (b == 0) {
        x = 1;
        if (a < 0) x = -1;
        y = 0;
        return a;
    }
    ll g = euclid_extended_recursive(b, a % b, x, y);
    x = x - (a / b) * y;
    swap(x, y);
    return g;
}

ll euclid_extended_iterative(ll a, ll b, ll &x, ll &y) {
    ll x_next = 0;
    ll y_next = 1;
    x = 1;
    y = 0;
    while (b != 0) {
        ll q = a / b;
        ll tmp = b;
        b = a % b;
        a = tmp;

        tmp = x_next;
        x_next = x - q * x_next;
        x = tmp;

        tmp = y_next;
        y_next = y - q * y_next;
        y = tmp;
    }
    return a;
}

ll linear_diophantine(const ll *coefficients, const ll n, ll *result, const ll target) {
    ll gcd = *coefficients;
    ll x;

    *result = 1;
    for (size_t i = 1; i < n; i++) {
        gcd = euclid_extended_iterative(gcd, coefficients[i], x, result[i]);
        for (size_t j = 0; j < i; j++) {
            result[j] *= x;
        }
    }
    if (target % gcd) {
        return 0;
    } else {
        ll mult = target / gcd;
        for (int j = 0; j < n; j++) {
            result[j] *= mult;
        }
        return 1;
    }
}

ll mul_inverse(ll num, ll mod_val) {
    ll inverse;
    ll temp;
    ll gcd = euclid_extended_iterative(num, mod_val, inverse, temp);
    if (gcd > 1) return 0;

    return ((inverse % mod_val) + mod_val) % mod_val;
}

ll chinese_reminder(const ll *remainders, const ll *modules, const ll num_equations) {
    ll total_mod = 1;
    ll gcd = 1;

    for (size_t i = 0; i < num_equations; i++) {
        gcd = euclid(gcd, modules[i]);
        if (gcd != 1) {
            return -1;
        }
        total_mod *= modules[i];
    }
    ll solution = 0;
    for (size_t i = 0; i < num_equations; i++) {
        ll mod_division = total_mod / modules[i];
        ll inverse = mul_inverse(mod_division, modules[i]);
        solution += (remainders[i] * mod_division * inverse);
    }
    return solution % total_mod;
}

ll chinese_remainder_iterative(const ll *remainders, const ll *modules, const ll n) {
    ll coeff1;
    ll coeff2;
    ll prev_module = *modules;
    ll prev_remainder = *remainders;
    ll solution = 0;
    ll modulo;
    for (size_t i = 1; i < n; i++) {
        modulo = modules[i];
        euclid_extended_iterative(prev_module, modulo, coeff1, coeff2);

        solution = remainders[i] * prev_module * coeff1 + prev_remainder * modulo * coeff2;
        prev_module *= modulo;
        prev_remainder = solution % prev_module;
    }
    return ((solution % prev_module) + prev_module) % prev_module;
}

ll cti(const char *ch) {
    ll result = 0;
    ll sign = 1;

    if (*ch == '-') {
        sign = -1;
        ch++;
    }

    while (*ch >= '0' && *ch <= '9') {
        result = result * 10 + (*ch - '0');
        ch++;
    }

    return result * sign;
}

bool is_prime(const ll num) {
    if (num <= 2) return num == 2;
    ull d = 3;
    while (d * d < num && (num % d)) d += 1;
    return d * d >= num;
}

ll lcm(ll x, ll y) {
    return (x / euclid(x, y)) * y;
}

#pragma endregion

#pragma region RPN

int performOperation(int operand1, int operand2, char operation) {
    switch (operation) {
        case '+':
            return operand1 + operand2;
        case '-':
            return operand1 - operand2;
        case '*':
            return operand1 * operand2;
        case '/':
            if (operand2 != 0) return operand1 / operand2;
            else {
                std::cout << "Error: division by zero\n";
                exit(1);
            }
        case '^':
            return (int) pow(operand1, operand2);
        case 'g':
            return (int) euclid(operand1, operand2);
        case 'l':
            return (int) lcm(operand1, operand2);
        default:
            std::cout << "Error: unknown operation\n";
            exit(1);
    }
}

int RPNCalculator(const char *expression) {
    stack *st = create_stack();
    int i = 0;
    size_t len = strlen(expression);

    while (i < len) {
        if (isdigit(expression[i]) || (expression[i] == '-' && isdigit(expression[i + 1]))) { // i < len -1
            int sign = 1;
            if (expression[i] == '-') {
                sign = -1;
                i++;
            }

            int operand = 0;
            while (isdigit(expression[i])) {
                operand = operand * 10 + (expression[i] - '0');
                i++;
            }
//            st.push(operand * sign);
            push(st, operand * sign);
        } else if (strchr("+-*/^gl", expression[i])) {
//            if (st.empty() || typeid(st.top()) != typeid(int)) {
            if (is_stack_empty(st) || typeid(stack_top(st)) != typeid(int)){
                std::cout << "Error: not enough operands\n";
                exit(1);
            }
//            int operand2 = st.top();
//            st.pop();
            int operand2 = stack_top(st);
            pop(st);
//            if (st.empty() || typeid(st.top()) != typeid(int)) {
            if (is_stack_empty(st) || typeid(stack_top(st)) != typeid(int)){
                std::cout << "Error: not enough operands\n";
                exit(1);
            }
//            int operand1 = st.top();
//            st.pop();
//            st.push(performOperation(operand1, operand2, expression[i]));
            int operand1 = stack_top(st);
            pop(st);
            push(st, performOperation(operand1, operand2, expression[i]));
            i++;
        } else {
            std::cout << "Error: Unknown character in the expression\n";
            exit(1);
        }
        i++;
    }

    if (is_stack_empty(st)) {
        std::cout << "Error: Invalid expression\n";
        exit(1);
    }

    int result = stack_top(st);
    pop(st);
    if (!is_stack_empty(st)) {
        std::cout << "Error: Invalid expression\n";
        exit(1);
    }
    return result;

}

short getPriority(char op) {
    switch (op) {
        case '(':
            return -1;
        case '^':
            return 4;
        case 'g':
        case 'l':
            return 3;
        case '*':
        case '/':
            return 2;
        case '+':
        case '-':
            return 1;
        default:
            return 0;
    }
}

void infixToPostfix(const char* infix, char* postfix) {
    stack* operatorStack = create_stack();
    int outputIdx = 0;
    size_t infixLen = strlen(infix);

    for (size_t i = 0; i < infixLen; i++) {
        char currentChar = infix[i];

        if (isdigit(currentChar) || (currentChar == '-' && (i + 1 < infixLen) && isdigit(infix[i + 1]))) {
            while(isdigit(currentChar) || currentChar == '-') {
                postfix[outputIdx++] = currentChar;
                i++;
                currentChar = infix[i];
            }
            postfix[outputIdx++] = ' ';
            i--;
        } else if (strchr("+-*/^gl", currentChar)) {
            while(!is_stack_empty(operatorStack) && getPriority(stack_top(operatorStack)) >= getPriority(currentChar)) {
                postfix[outputIdx++] = stack_top(operatorStack);
                postfix[outputIdx++] = ' ';
                pop(operatorStack);
            }
            push(operatorStack, (int)currentChar);
        } else if (currentChar == '(') push(operatorStack, (int)currentChar);
        else if (currentChar == ')') {
            while (!is_stack_empty(operatorStack) && stack_top(operatorStack) != '(') {
                postfix[outputIdx++] = stack_top(operatorStack);
                postfix[outputIdx++] = ' ';
                pop(operatorStack);
            }
            pop(operatorStack);
        }
    }

    while (!is_stack_empty(operatorStack)) {
        postfix[outputIdx++] = stack_top(operatorStack);
        postfix[outputIdx++] = ' ';
        pop(operatorStack);
    }

    postfix[outputIdx] = '\0';
}

#pragma endregion

#pragma region Qeueu

queue* create_queue()
{
    queue* q = (queue*)malloc(sizeof(queue));
    q->front = q->rear = nullptr;
    return q;
}

void enqueue(queue* q, int value)
{
    node* newNode = new node;
    newNode->val = value;
    newNode->next = nullptr;

    if (q->rear == nullptr) {
        q->front = q->rear = newNode;
        return;
    }

    q->rear->next = newNode;
    q->rear = newNode;
}

int dequeue(queue* q)
{
    if (q->front == nullptr) {
        return INT_MIN;
    }

    node* temp = q->front;
    int value = temp->val;
    q->front = q->front->next;

    if (q->front == nullptr) {
        q->rear = nullptr;
    }

    free(temp);
    return value;
}

int peek(queue* q) {
    if (q->front == nullptr) {
        return INT_MIN;
    }
    return q->front->val;
}

bool is_queue_empty(queue* q) {
    return q->front == nullptr;
}

void free_queue(queue* q) {
    while (!is_queue_empty(q)) {
        dequeue(q);
    }
    free(q);
}

#pragma endregion


