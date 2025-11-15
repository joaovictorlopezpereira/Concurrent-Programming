// Header
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.ArrayList;
import java.util.List;

class MyCallable implements Callable<Long> {
  public Long call() {
    long s = 0;
    for (long i = 1; i <= 100; i++) s++;
    return s;
  }
}

class PrimoCallable implements Callable<String> {
  private final long n;

  // Constructor
  public PrimoCallable(long n) { this.n = n; }

  // Verifies if a given number is prime
  private boolean ehPrimo(long x) {
    if (x <= 1) return false;
    if (x == 2) return true;
    if (x % 2 == 0) return false;
    long limite = (long)Math.sqrt(x);
    for (long i = 3; i <= limite; i += 2) {
      if (x % i == 0) return false;
    }
    return true;
  }

  // Returns a string containing whether n is prime or not
  public String call(){
    if(ehPrimo(n)) return n + " é primo";
    return n + " não é primo";
  }
}

// Classe TestPrimes (main)
public class TestPrimes{
  private static final int NTHREADS = 10;  // Number of threads
  private static final int NPRIMES = 1000; // Number of primes

  public static void main(String[] args){
    ExecutorService executor = Executors.newFixedThreadPool(NTHREADS);

    List<Future<String>> primos = new ArrayList<>();

    for(int i = 0 ; i < NPRIMES ; i++){
      Callable<String> worker = new PrimoCallable(i);
      Future<String> submit = executor.submit(worker);
      primos.add(submit);
    }

    for(Future<String> f:primos) {
      try{
        System.out.println(f.get());
      }
      catch(Exception e){
        e.printStackTrace();
      }
    }

    executor.shutdown();
  }
}
