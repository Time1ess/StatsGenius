import { Injectable } from '@angular/core';
import { of as observableOf } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class IconService {

  constructor() { }

  getIconForProcess(name: string) {
    return observableOf('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAYAAACqaXHeAAAAAXNSR0IArs4c6QAAAAlwSFlzAAAWJQAAFiUBSVIk8AAAAgtpVFh0WE1MOmNvbS5hZG9iZS54bXAAAAAAADx4OnhtcG1ldGEgeG1sbnM6eD0iYWRvYmU6bnM6bWV0YS8iIHg6eG1wdGs9IlhNUCBDb3JlIDUuNC4wIj4KICAgPHJkZjpSREYgeG1sbnM6cmRmPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjIj4KICAgICAgPHJkZjpEZXNjcmlwdGlvbiByZGY6YWJvdXQ9IiIKICAgICAgICAgICAgeG1sbnM6dGlmZj0iaHR0cDovL25zLmFkb2JlLmNvbS90aWZmLzEuMC8iPgogICAgICAgICA8dGlmZjpSZXNvbHV0aW9uVW5pdD4yPC90aWZmOlJlc29sdXRpb25Vbml0PgogICAgICAgICA8dGlmZjpDb21wcmVzc2lvbj4xPC90aWZmOkNvbXByZXNzaW9uPgogICAgICAgICA8dGlmZjpPcmllbnRhdGlvbj4xPC90aWZmOk9yaWVudGF0aW9uPgogICAgICAgICA8dGlmZjpQaG90b21ldHJpY0ludGVycHJldGF0aW9uPjI8L3RpZmY6UGhvdG9tZXRyaWNJbnRlcnByZXRhdGlvbj4KICAgICAgPC9yZGY6RGVzY3JpcHRpb24+CiAgIDwvcmRmOlJERj4KPC94OnhtcG1ldGE+Cg9FKpMAAAyiSURBVHgB7VtrbBzVFT4zs0/bsZ3Yxkn8SCAkxTiGPJSkVaGmLkSVIpqiEhU5Ki0SpVFFCqr6BxX+tPQHUtXSvAqRqqpUQjQthZAWhRJBiqK0dRQo5FGnCRAnjkmInfjtfczO7feNdzdje2d3djfYQeVIZ2fm3nvO+c65d+5r7op8Rv/fEdBmwH0dNivBFeBQ0n4E1wFwP9hKpk3LZboCUAdv2sBfBt8C5vMssB9MioOHwOfA74HfBL+RfMbl00tLAf1Z8MdglSdThrLU8amjEiD+CXgQnK/jk8tTB3VR56eCFgIlm/BkR4p9pk7qvqZpCdD9B1yss27y1E0b1yTNA6ojYDfwVyudNmjrmiIf0OwBXy0nc+l5BbZo85qhh4HEFfTtt9+u9u7dqw4ePKgefPBB13LMYxmWpUw2ncijzWuC6oHCdZhrbm5WfX19ykmZgsA0J1GGstDtxrRJ2zNOPwcCN5Bq586dTr/s+zNnzqiKioq0TGVlperq6ppSjrLZdCOPtmeU2BldBGcEGgqFVGdnp+2YZVnq6aefVps2bVJDQ0Nq7dq1aZm2tra08+fPn0+3mBMnTijqcNOftD2jHeLmLOBUdXW1unDhgu3c0aNHlaZptjPPPPOM2rx5c9qxhx56SJ09e1atX79eLV68WDU0NKjHHntMXb582daRzQbyiKFgKqYn5aJmQzbL0WhUyKTR0VFBJOz73t5eSSQS9j1/EBjZtWuXrFmzRrZu3Sp4RQStxJZJyacLT70hhu3gaV1EEcaN4BFwuiYz3bNHJ0UiEdXe3q5aWlrs18LZy8NxFY/H7XLOH8pm0jkpjRiIZdrpfljMCXDDhg1On2xHDx8+rAKBQFqW73lHR8eEcnygrBcbKEMs006/hsWcAA3DUNu2bUs719PTo1jjk2WZdu7cuXQ5ylB2cjmXZ2KZVuI+wgGwV4Cqre1O9a1vf0fVNTS6yjCPZVg2H91JLAXtbRQkBIPl4KPgBrBnCqHbrMJeUEWZJuGAJj52oyAT3ddYTMnAsJI+7AlF8u/OzkLNUjCXznlRoaNAFazMyWapukKTVUs0WbNYl5YFuiy4TqSmXJeysCZBvxKfIaInw2+hvk0MCtG4JsNjSi4OWtKFed6RLkv+ddKSQ/9V0jvARuFKxEJMeQeg0BawAsYOgZN1iLsklWKX7/Fv+qS91SeN1QDNEKNGFdjiVWn2e2MXT/mURMELpwo6tGrUTDZFzvRq8vzfTXnyD6aMcPdwKrHNrAK/PTUre0qhLYCvwBTnaSrs1+SLTbo0zoN32OmLRTzEOBkI+4IAJehOkgI+ZeuiTuoeiaSiliphX4mFmPKmQgOQ2s2dYrB3SMnaJ2Jy35cM+f46XVYu0kWHFQs1mUg4an+K5HgCw8XOPyVz6JQlO/5qyQtvJSTCrVN3csXkLoKWli0zS9465P0lS76dFcCeb1uLLvfe5pM7lqIfqMEbYQ//yGZFpirTbvvj2syYJl1YXew/asmfDpjyxhFLYtkdHxcUIaZXUw9er4UG4E4YeN2rEZarwJbm0oW6rLhRk6Z6Q+rQbc0Kj2sYGsN++CXspXUn5O1TSo6etmRgNB/tdtm78Lsvb6lCBF6orb7vhxWVanUwpCp0PVWX036lbWIgFmIqxJeC+oA7S0s23lNuSD96q564JZ1Y8ByJxuSYGZP346Z8ZJrSjy7/ynKnEGhXZDBiSiWGhnk+nyzy+6TZF5CWYEBuCgZlvl+XSkOXoURiI4q9cEXK213er0DPggVNGMMPo7rtBszu14cxy4CmBBJHMahfxkrvAgLRg+s5DPDncb0IvmwlZBCBGUW5GMbFRHJ1aGA1GICOEugoh6OzdUNqDEPmgutgbD6utXB8Nq4lmDykbJnQkRowIDoGUyvnd3Vx59gz5d0CdJ+1MaT5wnSUxFq2HRl/RDCU1GKKN88fkpXIS0126CvBJui4LaeJxckBSGcA0SOypg3cM6iIiU00w/Isa6FMlHaStpx9eFjXwqOayVbwuC3o8SdpxlvpY801ZVWj4Xd9mnGDeQWFN2FHKbfBUBU8KHG+pYmpEh/0lYzd2nzs4rDDXNbbjJMZN4m5UeOrQb0456mbjmZiN7te0lkhxEaMXsqnyuQVgO5Y4AFMxuz3MIA2qhfRClIAir0SA7GwbyA2YsxHZz6vwOeu0/V3WoKh8G2hkKwO+mUxeuIq9Mw0PGZF8I7qqFu+yXnFNQ+8rGfMEmEprIckjr6gDyPOSYxAHdG4HIhEMBpFxj62rOVQesKL4nwCwM7lp06l89Ar3xQIyvJwSB5eu1YqL/dLvOu0qBhnMW5BYMeX6rud2lL3dBGkMHcev8NVQ6cYFK2kXPx19TIwe7Zs/dtr8s5YRDpjUfkII8wkegLPT05Ky/joNQABSHeAb82kJVBRIadOnJCGqioZeuopGfnNFgDG1G8KWaIFSkRDC1KoOSwNx0ugCWsIpgQCcLJE9PJy0efMEb22Voy6OjEaGsRobLTv9epq6b50SRYtWSKxAR4qyUjvInU1OJYx15HodRjkiNbikJtwWwewc8rK0BWjL4YDboStUQnfe7+UPfIIVorx9C4xd4VtWUxsBKwhEOlxMIOy2aWlmErPkQ/dA0CsxPyPDOITkrwG4B5IubVpwZcdCYeTE/tUrU4wc+WBtc9aLIZoizazELESc84AuDrlUM7mn3VoKUPt69zF8EI5AuRFBW3NmjUrV1FiJvas5AX1zdDQlE0LtrWzZX8ieR5sEjOxZyUvAWiFhqyviufazwolv0y738guQszEnpW8BiCrEudnrqwFr2ImPrZ60ZYzAFlrFhbYpWcc+pzWx8awo5Ei9u7CZz9Gb5JzpMWoNHXMtkvl+4NPbV5EiJ0+uG6v5ApAI4TrMlji+JruYIaHh+2PnfiSI/6VKyV029fFwlitMEzhoyCXclzygQ17LM+gL68ktrjBwSk74BMwJRUSO33ozMuAozD32YB+Av8Zz6870+rq6lR/f3/6s5Z9k8DCd3RUJZCeuHTJvvJZ4ZxAsURb/ITuxJDERGyT0+mDK+XqAxZMkuzF86Pg0850HGcRHGxwJqG2sS7AeK1jlqhj6sorn7NNcCYqcH+irYsXsXM6kT7EI7ERo5Mm++DMc5/cJEvxa4uTfoSHM+BjzkS+j8eOTUhyZl/1e9rK0AcchyFiI0YnTfbBmZczAM4+Yickf5eUfgtXrlbStH///vT9J32TwRaxEBOJGIk1RU4fUmmerz9ASb5Te8GlDime8u4AM89mHm0ZGRkp9vXOKY+TJvYxmpTd5JVYiClFxErMxEYfCqZmSP4CnGni/V2kpwPA+5dffjmnA8UW2L179wSbSQzEMpmImdjpwydCWP7ZfUEaUGtrK1a5ZrE+uspTN23ArpPZ+RDLjNB6WOWULA3oueeec3Wg2AzqdtpK2iaGGaXtsJ4GNnfuXMXzfVebqJO6nbZwT9szTuxwXgOnwa1YsWLCeZ9ig8FzRdTptJG0SdvXBHFxvgecfh2WL1+ueDiyWKIO6oLuFNMGbeXcEECZ6SH1oqw693tje/sdemon0wZbU1Ojtm/frjh05UuU2bFjh6IOeJHmja26SVuxPfae3/Q46GZlZJexLv6Sf1/spYCpXg0qc3dQbfteQNVWjh+JTQFftmyZ2rJlizp58qTCQsY1FsxjGZadVOu2TuqmDdqiTdo2XzTWueHzku5cq3opb5d587cS+kJF8Jd+QzZhgYf9TXzpQR1xb9OPAxAf9Gjyqz1xnOuxJhxuKsVmZlNTk+AYvNTX1wtOjNv6BrBq7O7uluPHj9uMCVUaCw9btbfq8sjdfrlhvpI4DlCkbeGwFbcFsAJ/9qPB6KPXPyBYeuZHBQUg/pLvK74yYx/NEYCFcz1spGynJB+OuGgIzOnzmrzyT0v2HDLl3x/mPOk1LoxfOr3sek3uXuWTr31el4Xo/BW2/k0csSHZv/jRkweq+PfL6GDirtA3zH12gTx+xjXmIcCiw89LbSjs/7GlGasBbC4QlQFLUGkW5938cGN/N8NHYoUWouNsUPTMx1r8va5E7N3TEjl+1hw51aMG3++RPupbNF+qbpyvld/c4Cu9daGEbllgBBqvU36cFQrii7qFc4T4lDyuE8UtTemm0iWKtGEE+rwuiY7IaPxnZe1ygfryoYIC4DTQuVtm1Y9IiRGWsBbFZzp84UYICFgBJL9pm8oP5pdxHxpMosw0teHYcFyi9RvgBKj7jxIs80vQp8oCujHs00x7+8TQ4tiL1PHFnd/cUjqpJyixxJiMdZfK6E3r7X+cOiF9dv9ZBPKIwP8Ab3tw+SD7SPMAAAAASUVORK5CYII=');
  }
}
