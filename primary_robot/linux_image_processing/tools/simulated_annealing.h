/*
 * simulated_annealing.h
 *
 *  Created on: 28 avr. 2015
 *      Author: ludo6431
 */

#ifndef TOOLS_SIMULATED_ANNEALING_H_
#define TOOLS_SIMULATED_ANNEALING_H_

#include <functional>
#include <random>

#define SA_DEBUG 2

#if SA_DEBUG > 0
#   include <iostream>
#endif

// To find a status with lower energy according to the given condition
template<typename status, typename count, typename energy>
status simulated_annealing(status i_old, energy T, energy alpha, count c, count max_without_amelioration,
        std::function<energy(status const&)> const& ef,
        std::function<status(status const&, count rem_c)> const& nf) {

    energy e_old = ef(i_old);

    status i_best = i_old;
    energy e_best = e_old;

    static std::random_device g;
    std::uniform_real_distribution<energy> rf(0, 1);

    count steps_without_amelioration = 0;

    for (; c > 0; --c) {
        status i_new = nf(i_old, c);
        energy e_new = ef(i_new);

        if (e_new < e_best) {
            i_best = i_new;
            e_best = e_new;
            steps_without_amelioration = 0;

#if SA_DEBUG > 0
            std::cout << "new best: " << i_new << std::endl;
#endif
        }
        else {
            steps_without_amelioration++;

            if (steps_without_amelioration > max_without_amelioration) {
                i_old = i_best;
                e_old = e_best;

#if SA_DEBUG > 1
                std::cout << "ret2best: " << i_new << std::endl;
#endif
                continue;
            }
        }

        if (e_new < e_old || std::exp((e_old - e_new) / T) > rf(g)) {
            i_old = std::move(i_new);
            e_old = std::move(e_new);

#if SA_DEBUG > 2
            std::cout << "new curr: " << i_old << std::endl;
#endif
        }

        T *= alpha;
    }
    return (i_best);
}

#endif /* TOOLS_SIMULATED_ANNEALING_H_ */
