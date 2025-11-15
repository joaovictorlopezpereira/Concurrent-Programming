// Header
import java.util.LinkedList;

// Class Fila Tarefas
class FilaTarefas {
  private final int nThreads;
  private final MyPoolThreads[] threads;
  private final LinkedList<Runnable> queue;
  private boolean shutdown;

  // Initializes queue, creates and initializes threads
  public FilaTarefas(int nThreads) {
    this.shutdown = false;
    this.nThreads = nThreads;
    queue = new LinkedList<Runnable>();
    threads = new MyPoolThreads[nThreads];
    for(int i=0; i<nThreads; i++){
      threads[i] = new MyPoolThreads();
      threads[i].start();
    }
  }

  // Adds a task to the queue and wakes up a thread to execute it
  public void execute(Runnable r) {
    synchronized(queue) {
      if (this.shutdown) return;
      queue.addLast(r);
      queue.notify();
    }
  }

  // Shutsdown the pool
  public void shutdown() {
    synchronized(queue) {
      this.shutdown=true;
      queue.notifyAll();
    }
    for(int i=0 ; i<nThreads ; i++){
      try{threads[i].join();}
      catch(InterruptedException e){return;}
    }
  }

  private class MyPoolThreads extends Thread {
    // Method executed by the thread
    public void run() {
      Runnable r;
      while(true){
        synchronized(queue) {
          while(queue.isEmpty() && (!shutdown)){
            try{ queue.wait();}
            catch(InterruptedException ignored){}
          }
          if (queue.isEmpty()) return;
          r = (Runnable) queue.removeFirst();
        }
        try{r.run();}
        catch(RuntimeException e){}
      }
    }
  }
}

// CLass Primo
class Primo implements Runnable {
  private final long n;

  // Constructor class
  public Primo(long n) {
    this.n = n;
  }

  // Returns true or false whether x is a prime or not
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

  // Prints whether n is a prime or not (method executed by the thread)
  public void run() {
    if (ehPrimo(n))
      System.out.println(n + " é primo");
    else
      System.out.println(n + " não é primo");
  }
}

// Class MyPool (main)
class MyPool {
  private static final int NTHREADS = 10;

  public static void main (String[] args) {
    // Creates the pool
    FilaTarefas pool = new FilaTarefas(NTHREADS);

    // Lauches the runnable objects using the pool
    for (int i = 0; i < 25; i++) {
      Runnable primo = new Primo(i);
      pool.execute(primo);
    }

    // Waits for all threads to finish to finally shutdown the pool
    pool.shutdown();
    System.out.println("Terminou");
  }
}
