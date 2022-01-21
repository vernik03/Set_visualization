Основною метою лабораторної була реалізація трьох математичних алгоритмів для множини точок на площині та зображення процесу роботи алгоритмів та фінальних результатів роботи. 

Керування роботою програми здійснюється за допомогою клавіш, інструкція виводиться в консоль, візуалізація зроблена за допомогою бібліотеки SFML. 

1) Опукла оболонка
Опуклою оболонкою скінченного набору точок на площині є опуклий плоский  багатокутник, причому його вершини є підмножиною похідного набору точок.
Принцип роботи: 
~ Знаходимо найлівішу з найнижчих точок площини.
~ Рахуємо кути між прямою, утвореною знайденою точкою і знайденою точкою, зміщену на найменше можливе значення по осі Х вліво та між усіма іншими точками.
~ Знаходимо найменший правий кут та точку, яка його утворює. Вона буде наступною точкою оболонки.
~ Тепер утворюємо пряму за допомогою останніх двох точок (найлівішої з найнижчих та наступної, знайденої тільки що), знов шукаємо кути.
~ Знаходимо найменший правий кут та точку, яка його утворює. Вона буде наступною точкою оболонки.
~ Зміщуємося в масиві точок оболонки на одну, знов використовуємо останні дві.
~ Переходимо до пункту 4 або закінчуємо підрахунок, якщо остання знайдена точка є найпершою.

2) Тріангуляція Делоне
Умова Делоне стверджує, що мережа трикутників є тріангуляцією Делоне, якщо всі описані кола трикутників пусті. 
Зроблена за допомогою алгоритма Боуера — Ватсона.
Основна ідея – додавання точок по одній та видалення трикутників, точки яких знаходяться в середині описаних кіл наявних трикутників.

3) Діаграма Вороного
У найпростішому випадку ми маємо множину точок площини S, які називаються вершинами діаграми Вороного. Кожній вершині s належить комірка Вороного, також відома як комірка Діріхле, утворена з усіх точок ближчих до s ніж до будь-якої іншої вершини.
Використовуючи тріангуляція Делоне, з’єднуємо центри описаних кіл тих трикутників, які мають спільні ребра.
 
Для анімації зроблена окрема функція, яка відображає кадри в процесі роботи алгоритма: додавання точок до опуклої оболонки, знаходження трикутників, з’єднання точок діаграми.

Користувач може перемикати алгоритми за допомогою клавіш «1», «2» та «3». Додавати точки кліком миші, за допомогою рандомайзеру або файлу. Також можна змінювати швидкість анімації клавішами «-» та «+».