// Задача: выполнилнить проектировочные расчеты
// двухступенчатого обводного редуктора
// согласно технического задания и ГОСТов
// вывод программы должен быть copy paste friendly

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define EPS 1E-4

#define Pin 1850
#define Nin 1650
#define Nout 155
#define Th 1475
#define Cmr 1.2
#define Zsum 84
#define Sh 1.2

#define KD 1.25
#define BEARING_LOSSES 0.995
#define GEARING_LOSSES 0.97

#define HB 300
#define SIGMA_V 1100
#define SIGMA_T 950
#define SIGMA_1 510
#define TAU_T 570
#define TAU_1 280
#define KCU 100

#define BETA 0 /* прямозубая передача */
#define REL_RING_GEAR 0.2
#define FAST_CONSOLE 4.4
#define SLOW_ASYMMETRIC 3.1

#if 0
#define Tin 10707.58
#define T1 13317.55
#define T2 50909.93
#define T3 133452.61
#endif
#define SGM_HP 558.0
#define PHI 0.2


double binpow(double x, unsigned k)
{
    double result = 1;

    while (k) {

        if (k & 1)
            result *= x;
        x *= x;
        k >>= 1;
    }

    return result;
}

double root(double a, unsigned k)
{
    double x = 1, nx;
    unsigned p = k - 1;

    while (1) {

        nx = (a / binpow(x, p) + p * x) / k;
        if (fabs(x - nx) < EPS) break;
        x = nx;
    }

    return x;
}


double module_revision(double m)
{
    double gost[] = { 0.6, 0.8, 1, 1.25, 1.5, 2, 2.5, 3, 4 };
    unsigned i, size = sizeof(gost) / sizeof(double);

    for (i = 0; i < size; i++) if (m < gost[i]) return gost[i];

    return 0;
}


struct wheel_geometry_t {
    double d;
    double da;
    double df;
    double aw;
    double bw;
};

#define wheel_geom struct wheel_geometry_t
wheel_geom wheel_geometry(double m, double z)
{
    wheel_geom wheel;
    wheel.d = m * z;
    wheel.da = m * (z + 2);
    wheel.df = m * (z - 2.5);
    return wheel;
}

void print_wheel_geom(struct wheel_geometry_t);
void print_wheel_force(double T, double d, double bw);

/*
#define M__T 1716226113.87654

void main_test()
{
    unsigned i, j = 1E6;

    printf("13 ^ 17 = %f\n", binpow(13.0, 17));
    printf("1716226113.87654 root 23 = %f\n", root(M__T, 23));

    for (i = 0; i < j; i++) {
        binpow(13.0, 17);
    }
    for (i = 0; i < j; i++) {
        root(M__T, 23);
    }
}
*/

void steel_gear()
{
    double i_12, i_23, i_sum,
        u_12, u_23, u_sum, acc,
        Tin, T1, T2, T3,
        Hlim_a, Hlim_b, Hlim, Sigma_HP,
        aw, bw, _m, m, d1, d2, d3;
    int z1, z2, z3, N_h0, N_he;
    wheel_geom input_gear, middle_gear, output_gear;

    i_sum = (double) Nin / Nout;
    i_12 = Cmr * root(i_sum, 2);
    i_23 = i_sum / i_12;

    z1 = round( (double) Zsum / (1 + i_12) );
    z2 = round(z1 * i_12);
    z3 = round(z2 * i_23);

    u_12 = (double) z2 / z1;
    u_23 = (double) z3 / z2;
    u_sum = u_12 * u_23;
    acc = fabs(i_sum - u_sum) / i_sum * 100;

    Tin = (double) 9550 * Pin / Nin;
    T1 = Tin * KD * BEARING_LOSSES,
    T2 = T1 * u_12 * GEARING_LOSSES,
    T3 = T2 * u_23 * BEARING_LOSSES * GEARING_LOSSES;

    Hlim_a = 1.4 * SIGMA_T;
    Hlim_b = (double) 2 * HB + 70;
    N_h0 = 30 * root(binpow(HB, 24), 10);
    N_he = 60 * Nin * Th;
    Hlim = Hlim_b * root( (double) N_h0 / N_he, 6);
    Hlim = Hlim > Hlim_a ? Hlim_a : Hlim;
    Hlim = Hlim < Hlim_b ? Hlim_b : Hlim;
    Sigma_HP = Hlim / Sh;

    /* только для колес из стали одинаковой марки */
    aw = u_12 * binpow(Sigma_HP, 2) * REL_RING_GEAR;
    aw = 48.7 * (u_12 + 1) * root(T1 / aw , 3);

    /* только для прямозубых колес */
    _m = 2 * aw / (z1 + z2);
    m = module_revision(_m);

    /* только для сплошных валов */
    d1 = FAST_CONSOLE * root(T1 / SIGMA_1, 3);
    d2 = SLOW_ASYMMETRIC * root(T2 / SIGMA_1, 3);
    d3 = SLOW_ASYMMETRIC * root(T3 / SIGMA_1, 3);

    input_gear = wheel_geometry(m, z1);
    middle_gear = wheel_geometry(m, z2);
    output_gear = wheel_geometry(m, z3);


    printf("Общее передаточное число: %.3f\n\n", i_sum);

    printf("Разбивка передаточных отношений по ступеням\n");
    printf("i_12 = %.3f\ni_23 = %.3f\n\n", i_12, i_23);

    printf("Вычисление числа зубьев\n");
    printf("z1 = %d\nz2 = %d\nz3 = %d\n\n", z1, z2, z3);

    printf("Проверка передаточных отношений\n");
    printf("u12 = %.3f\nu23 = %.3f\n", u_12, u_23);
    printf("Общее передаточное отношение: %.3f\n", u_sum);
    printf("Погрешность передаточных чисел: %.2f%%\n\n", acc);

    printf("Определение крутящих моментов на валах (Н * мм)\n");
    printf("Tin = %.3f\n", Tin);
    printf("T1 = %.3f\nT2 = %.3f\nT3 = %.3f\n\n", T1, T2, T3);

    printf("Определение предельных и допускаемых контактных\n");
    printf("напряжений на поверхностях зубьев шестерни и колеса\n");
    printf("Предельные контактные напряжения: %.1f МПа\n", Hlim);
    printf("Коэффициент безопасности: %.2f\n", Sh);
    printf("Допускаемые контактные напряжения: %.2f МПа\n\n", Sigma_HP);

    printf("Определение в первом приближении межосевого\n");
    printf("расстояния цилиндрической зубчатой пары\n");
    printf("aw = %.1f мм\n\n", aw);

    printf("Определение модуля зубчатой передачи\n");
    printf("Приближенный модуль зацепления: %.2f мм\n", _m);
    printf("Уточненный по ГОСТ 9563—80 модуль зацепления: ");
    printf("%.2f мм\n\n", m);

    printf("Проектировочный расчет диаметров валов\n");
    printf("разрабатываемого узла редуктора\n");
    printf("d1 = %.3f мм\nd2 = %.3f мм\nd3 = %.3f мм\n\n", d1, d2, d3);

    printf("Определение основных геометрических размеров\n");
    printf("цилиндрических зубьев разрабатываемого узла редуктора\n");
    printf("для входного колеса (шестерни)\n");
    print_wheel_geom(input_gear);
    printf("для промежуточного колеса\n");
    print_wheel_geom(middle_gear);
    printf("для выходного колеса\n");
    print_wheel_geom(output_gear);


    aw = m / 2 * (z1 + z2);
    bw = aw * REL_RING_GEAR;


    printf("межосевое расстояние aw — %.2f мм\n", aw);
    printf("рабочая ширина зуба bw — %.2f мм\n\n", bw);

    printf("Расчетные контактные напряжения на зубьях\n");
    printf("для входного колеса (шестерни)\n");
    print_wheel_force(T1, m * z1, bw);
    printf("для промежуточного колеса\n");
    print_wheel_force(T2, m * z2, bw);
    printf("для выходного колеса\n");
    print_wheel_force(T3, m * z3, bw);

}

void print_wheel_geom(wheel_geom wheel)
{
    printf("делительный диаметр — %.2f мм\n", wheel.d);
    printf("диаметр окружности вершин — %.2f мм\n", wheel.da);
    printf("диаметр окружности впадин — %.2f мм\n\n", wheel.df);
}

void print_wheel_force(double T, double d, double bw)
{
    double force = 2 * T / d, relative = force / bw;
    printf("окружная сила Ft = %.2f Н\n", force);
    printf("удельная окружная сила wt = %.2f Н / мм\n\n", relative);
}

int main()
{
    steel_gear();
    return 0;
}
