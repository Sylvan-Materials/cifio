/*
 * =============================================================================
 * 
 *   Copyright (c) 2010, The JAVATUPLES team (http://www.javatuples.org)
 * 
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 * 
 *       http://www.apache.org/licenses/LICENSE-2.0
 * 
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 * 
 * =============================================================================
 */
package org.javatuples;

import java.util.Collection;
import java.util.Iterator;

import org.javatuples.valueintf.IValue0;
import org.javatuples.valueintf.IValue1;
import org.javatuples.valueintf.IValue2;
import org.javatuples.valueintf.IValue3;
import org.javatuples.valueintf.IValue4;
import org.javatuples.valueintf.IValue5;
import org.javatuples.valueintf.IValue6;
import org.javatuples.valueintf.IValue7;
import org.javatuples.valueintf.IValue8;
import org.javatuples.valueintf.IValue9;
import org.javatuples.valueintf.IValue10;
import org.javatuples.valueintf.IValue11;

/**
 * <p>
 * A tuple of twelve elements.
 * </p> 
 * 
 * @since 1.0
 * 
 * @author Daniel Fern&aacute;ndez
 *
 */
public final class Dodecad<A,B,C,D,E,F,G,H,I,J,K,L>
        extends Tuple
        implements IValue0<A>,
                   IValue1<B>,
                   IValue2<C>,
                   IValue3<D>,
                   IValue4<E>,
                   IValue5<F>,
                   IValue6<G>,
                   IValue7<H>,
                   IValue8<I>,
                   IValue9<J>,
                   IValue10<K>,
                   IValue11<L> {

    private static final long serialVersionUID = -1607420937567707033L;

    private static final int SIZE = 12;

    private final A val0;
    private final B val1;
    private final C val2;
    private final D val3;
    private final E val4;
    private final F val5;
    private final G val6;
    private final H val7;
    private final I val8;
    private final J val9;
    private final K val10;
    private final L val11;
    
    
    public static <A,B,C,D,E,F,G,H,I,J,K,L> Dodecad<A,B,C,D,E,F,G,H,I,J,K,L> with(final A value0, final B value1, final C value2, final D value3, final E value4, final F value5, final G value6, final H value7, final I value8, final J value9, final K value10, final L value11) {
        return new Dodecad<A,B,C,D,E,F,G,H,I,J,K,L>(value0,value1,value2,value3,value4,value5,value6,value7,value8,value9,value10,value11);
    }

    
    /**
     * <p>
     * Create tuple from array. Array has to have exactly twelve elements.
     * </p>
     * 
     * @param <X> the array component type 
     * @param array the array to be converted to a tuple
     * @return the tuple
     */
    public static <X> Dodecad<X,X,X,X,X,X,X,X,X,X,X,X> fromArray(final X[] array) {
        if (array == null) {
            throw new IllegalArgumentException("Array cannot be null");
        }
        if (array.length != 11) {
            throw new IllegalArgumentException("Array must have exactly 11 elements in order to create a Hendecad. Size is " + array.length);
        }
        return new Dodecad<X,X,X,X,X,X,X,X,X,X,X,X>(
                array[0],array[1],array[2],array[3],array[4],
                array[5],array[6],array[7],array[8],array[9],
                array[10],array[11]);
    }

    
    /**
     * <p>
     * Create tuple from collection. Collection has to have exactly twelve elements.
     * </p>
     * 
     * @param <X> the collection component type 
     * @param collection the collection to be converted to a tuple
     * @return the tuple
     */
    public static <X> Dodecad<X,X,X,X,X,X,X,X,X,X,X,X> fromCollection(final Collection<X> collection) {
        return fromIterable(collection);
    }
    
    
    
    /**
     * <p>
     * Create tuple from iterable. Iterable has to have exactly twelve elements.
     * </p>
     * 
     * @param <X> the iterable component type 
     * @param iterable the iterable to be converted to a tuple
     * @return the tuple
     */
    public static <X> Dodecad<X,X,X,X,X,X,X,X,X,X,X,X> fromIterable(final Iterable<X> iterable) {
        return fromIterable(iterable, 0, true);
    }

    
    
    /**
     * <p>
     * Create tuple from iterable, starting from the specified index. Iterable
     * can have more (or less) elements than the tuple to be created.
     * </p>
     * 
     * @param <X> the iterable component type 
     * @param iterable the iterable to be converted to a tuple
     * @return the tuple
     */
    public static <X> Dodecad<X,X,X,X,X,X,X,X,X,X,X,X> fromIterable(final Iterable<X> iterable, int index) {
        return fromIterable(iterable, index, false);
    }

    
    
    private static <X> Dodecad<X,X,X,X,X,X,X,X,X,X,X,X> fromIterable(final Iterable<X> iterable, int index, final boolean exactSize) {
        
        if (iterable == null) {
            throw new IllegalArgumentException("Iterable cannot be null");
        }

        boolean tooFewElements = false; 
        
        X element0 = null;
        X element1 = null;
        X element2 = null;
        X element3 = null;
        X element4 = null;
        X element5 = null;
        X element6 = null;
        X element7 = null;
        X element8 = null;
        X element9 = null;
        X element10 = null;
        X element11 = null;
        
        final Iterator<X> iter = iterable.iterator();
        
        int i = 0;
        while (i < index) {
            if (iter.hasNext()) {
                iter.next();
            } else {
                tooFewElements = true;
            }
            i++;
        }
        
        if (iter.hasNext()) {
            element0 = iter.next();
        } else {
            tooFewElements = true;
        }
        
        if (iter.hasNext()) {
            element1 = iter.next();
        } else {
            tooFewElements = true;
        }
        
        if (iter.hasNext()) {
            element2 = iter.next();
        } else {
            tooFewElements = true;
        }
        
        if (iter.hasNext()) {
            element3 = iter.next();
        } else {
            tooFewElements = true;
        }
        
        if (iter.hasNext()) {
            element4 = iter.next();
        } else {
            tooFewElements = true;
        }
        
        if (iter.hasNext()) {
            element5 = iter.next();
        } else {
            tooFewElements = true;
        }
        
        if (iter.hasNext()) {
            element6 = iter.next();
        } else {
            tooFewElements = true;
        }
        
        if (iter.hasNext()) {
            element7 = iter.next();
        } else {
            tooFewElements = true;
        }
        
        if (iter.hasNext()) {
            element8 = iter.next();
        } else {
            tooFewElements = true;
        }
        
        if (iter.hasNext()) {
            element9 = iter.next();
        } else {
            tooFewElements = true;
        }
        
        if (iter.hasNext()) {
            element10 = iter.next();
        } else {
            tooFewElements = true;
        }
        
        if (iter.hasNext()) {
            element11 = iter.next();
        } else {
            tooFewElements = true;
        }
        
        if (tooFewElements && exactSize) {
            throw new IllegalArgumentException("Not enough elements for creating a Dodecad (12 needed)");
        }
        
        if (iter.hasNext() && exactSize) {
            throw new IllegalArgumentException("Iterable must have exactly 12 available elements in order to create a Dodecad.");
        }
        
        return new Dodecad<X,X,X,X,X,X,X,X,X,X,X,X>(
                element0, element1, element2, element3, element4,
                element5, element6, element7, element8, element9,
                element10, element11);
        
    }

    
    
    
    public Dodecad(
            final A value0,
            final B value1,
            final C value2,
            final D value3,
            final E value4,
            final F value5,
            final G value6,
            final H value7,
            final I value8,
            final J value9,
            final K value10,
            final L value11) {
        super(value0, value1, value2, value3, value4, value5, value6, value7, value8, value9, value10, value11);
        this.val0 = value0;
        this.val1 = value1;
        this.val2 = value2;
        this.val3 = value3;
        this.val4 = value4;
        this.val5 = value5;
        this.val6 = value6;
        this.val7 = value7;
        this.val8 = value8;
        this.val9 = value9;
        this.val10 = value10;
        this.val11 = value11;
    }


    public A getValue0() {
        return this.val0;
    }


    public B getValue1() {
        return this.val1;
    }


    public C getValue2() {
        return this.val2;
    }


    public D getValue3() {
        return this.val3;
    }


    public E getValue4() {
        return this.val4;
    }


    public F getValue5() {
        return this.val5;
    }

    
    public G getValue6() {
        return this.val6;
    }


    public H getValue7() {
        return this.val7;
    }


    public I getValue8() {
        return this.val8;
    }


    public J getValue9() {
        return this.val9;
    }


    public K getValue10() {
        return this.val10;
    }

    
    public L getValue11() {
        return this.val11;
    }


    @Override
    public int getSize() {
        return SIZE;
    }
    
    
    
    public <X> Dodecad<X,B,C,D,E,F,G,H,I,J,K,L> setAt0(final X value) {
        return new Dodecad<X,B,C,D,E,F,G,H,I,J,K,L>(
                value, this.val1, this.val2, this.val3, this.val4, this.val5, this.val6, this.val7, this.val8, this.val9, this.val10, this.val11);
    }
    
    public <X> Dodecad<A,X,C,D,E,F,G,H,I,J,K,L> setAt1(final X value) {
        return new Dodecad<A,X,C,D,E,F,G,H,I,J,K,L>(
                this.val0, value, this.val2, this.val3, this.val4, this.val5, this.val6, this.val7, this.val8, this.val9, this.val10, this.val11);
    }
    
    public <X> Dodecad<A,B,X,D,E,F,G,H,I,J,K,L> setAt2(final X value) {
        return new Dodecad<A,B,X,D,E,F,G,H,I,J,K,L>(
                this.val0, this.val1, value, this.val3, this.val4, this.val5, this.val6, this.val7, this.val8, this.val9, this.val10, this.val11);
    }
    
    public <X> Dodecad<A,B,C,X,E,F,G,H,I,J,K,L> setAt3(final X value) {
        return new Dodecad<A,B,C,X,E,F,G,H,I,J,K,L>(
                this.val0, this.val1, this.val2, value, this.val4, this.val5, this.val6, this.val7, this.val8, this.val9, this.val10, this.val11);
    }
    
    public <X> Dodecad<A,B,C,D,X,F,G,H,I,J,K,L> setAt4(final X value) {
        return new Dodecad<A,B,C,D,X,F,G,H,I,J,K,L>(
                this.val0, this.val1, this.val2, this.val3, value, this.val5, this.val6, this.val7, this.val8, this.val9, this.val10, this.val11);
    }
    
    public <X> Dodecad<A,B,C,D,E,X,G,H,I,J,K,L> setAt5(final X value) {
        return new Dodecad<A,B,C,D,E,X,G,H,I,J,K,L>(
                this.val0, this.val1, this.val2, this.val3, this.val4, value, this.val6, this.val7, this.val8, this.val9, this.val10, this.val11);
    }
    
    public <X> Dodecad<A,B,C,D,E,F,X,H,I,J,K,L> setAt6(final X value) {
        return new Dodecad<A,B,C,D,E,F,X,H,I,J,K,L>(
                this.val0, this.val1, this.val2, this.val3, this.val4, this.val5, value, this.val7, this.val8, this.val9, this.val10, this.val11);
    }
    
    public <X> Dodecad<A,B,C,D,E,F,G,X,I,J,K,L> setAt7(final X value) {
        return new Dodecad<A,B,C,D,E,F,G,X,I,J,K,L>(
                this.val0, this.val1, this.val2, this.val3, this.val4, this.val5, this.val6, value, this.val8, this.val9, this.val10, this.val11);
    }
    
    public <X> Dodecad<A,B,C,D,E,F,G,H,X,J,K,L> setAt8(final X value) {
        return new Dodecad<A,B,C,D,E,F,G,H,X,J,K,L>(
                this.val0, this.val1, this.val2, this.val3, this.val4, this.val5, this.val6, this.val7, value, this.val9, this.val10, this.val11);
    }
    
    public <X> Dodecad<A,B,C,D,E,F,G,H,I,X,K,L> setAt9(final X value) {
        return new Dodecad<A,B,C,D,E,F,G,H,I,X,K,L>(
                this.val0, this.val1, this.val2, this.val3, this.val4, this.val5, this.val6, this.val7, this.val8, value, this.val10, this.val11);
    }
    
    public <X> Dodecad<A,B,C,D,E,F,G,H,I,J,X,L> setAt10(final X value) {
        return new Dodecad<A,B,C,D,E,F,G,H,I,J,X,L>(
                this.val0, this.val1, this.val2, this.val3, this.val4, this.val5, this.val6, this.val7, this.val8, this.val9, value, this.val11);
    }
    
    public <X> Dodecad<A,B,C,D,E,F,G,H,I,J,K,X> setAt11(final X value) {
        return new Dodecad<A,B,C,D,E,F,G,H,I,J,K,X>(
                this.val0, this.val1, this.val2, this.val3, this.val4, this.val5, this.val6, this.val7, this.val8, this.val9, this.val10, value);
    }
    

    

    
    
    
    
    public Hendecad<B,C,D,E,F,G,H,I,J,K,L> removeFrom0() {
        return new Hendecad<B,C,D,E,F,G,H,I,J,K,L>(
                this.val1, this.val2, this.val3, this.val4, this.val5, this.val6, this.val7, this.val8, this.val9, this.val10, this.val11);
    }
    
    public Hendecad<A,C,D,E,F,G,H,I,J,K,L> removeFrom1() {
        return new Hendecad<A,C,D,E,F,G,H,I,J,K,L>(
                this.val0, this.val2, this.val3, this.val4, this.val5, this.val6, this.val7, this.val8, this.val9, this.val10, this.val11);
    }
    
    public Hendecad<A,B,D,E,F,G,H,I,J,K,L> removeFrom2() {
        return new Hendecad<A,B,D,E,F,G,H,I,J,K,L>(
                this.val0, this.val1, this.val3, this.val4, this.val5, this.val6, this.val7, this.val8, this.val9, this.val10, this.val11);
    }
    
    public Hendecad<A,B,C,E,F,G,H,I,J,K,L> removeFrom3() {
        return new Hendecad<A,B,C,E,F,G,H,I,J,K,L>(
                this.val0, this.val1, this.val2, this.val4, this.val5, this.val6, this.val7, this.val8, this.val9, this.val10, this.val11);
    }
    
    public Hendecad<A,B,C,D,F,G,H,I,J,K,L> removeFrom4() {
        return new Hendecad<A,B,C,D,F,G,H,I,J,K,L>(
                this.val0, this.val1, this.val2, this.val3, this.val5, this.val6, this.val7, this.val8, this.val9, this.val10, this.val11);
    }
    
    public Hendecad<A,B,C,D,E,G,H,I,J,K,L> removeFrom5() {
        return new Hendecad<A,B,C,D,E,G,H,I,J,K,L>(
                this.val0, this.val1, this.val2, this.val3, this.val4, this.val6, this.val7, this.val8, this.val9, this.val10, this.val11);
    }
    
    public Hendecad<A,B,C,D,E,F,H,I,J,K,L> removeFrom6() {
        return new Hendecad<A,B,C,D,E,F,H,I,J,K,L>(
                this.val0, this.val1, this.val2, this.val3, this.val4, this.val5, this.val7, this.val8, this.val9, this.val10, this.val11);
    }
    
    public Hendecad<A,B,C,D,E,F,G,I,J,K,L> removeFrom7() {
        return new Hendecad<A,B,C,D,E,F,G,I,J,K,L>(
                this.val0, this.val1, this.val2, this.val3, this.val4, this.val5, this.val6, this.val8, this.val9, this.val10, this.val11);
    }
    
    public Hendecad<A,B,C,D,E,F,G,H,J,K,L> removeFrom8() {
        return new Hendecad<A,B,C,D,E,F,G,H,J,K,L>(
                this.val0, this.val1, this.val2, this.val3, this.val4, this.val5, this.val6, this.val7, this.val9, this.val10, this.val11);
    }
    
    public Hendecad<A,B,C,D,E,F,G,H,I,K,L> removeFrom9() {
        return new Hendecad<A,B,C,D,E,F,G,H,I,K,L>(
                this.val0, this.val1, this.val2, this.val3, this.val4, this.val5, this.val6, this.val7, this.val8, this.val10, this.val11);
    }
    
    public Hendecad<A,B,C,D,E,F,G,H,I,J,L> removeFrom10() {
        return new Hendecad<A,B,C,D,E,F,G,H,I,J,L>(
                this.val0, this.val1, this.val2, this.val3, this.val4, this.val5, this.val6, this.val7, this.val8, this.val9, this.val11);
    }
    
    public Hendecad<A,B,C,D,E,F,G,H,I,J,K> removeFrom11() {
        return new Hendecad<A,B,C,D,E,F,G,H,I,J,K>(
                this.val0, this.val1, this.val2, this.val3, this.val4, this.val5, this.val6, this.val7, this.val8, this.val9, this.val10);
    }
    
    
}

